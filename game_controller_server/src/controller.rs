use std::f64::MAX;
use std::sync::Mutex;
use actix_web::web::{Json, to};

use chrono::format::Item;
use lazy_static::lazy_static;
use redis::{Client, RedisResult};
use serde::{Deserialize, Serialize};

use crate::server::server_ctl::{ServerCtl, ServerOwner};
use crate::store::store_ctl;
use crate::store::store_data::{Login_Data, ServerAddr, ServerHeartbeat, ServerInfo, ServerRunningStatus, Player, PlayerScore};
use crate::user::account_ctl::{AccountCtl, RegisterResult};
use crate::user::login_sign_ctl::{LoginSignCtl, SignLoginFailResult};

// 第一次调用的时候会被初始化
lazy_static! {
    pub static ref  SERVERCONTROLLER: Mutex<ServerController> = Mutex::new(ServerController::new());
}


// 服务器控制
pub struct ServerController {
    user: AccountCtl,
    server: ServerCtl,
    login: LoginSignCtl,
}
#[derive(Serialize,Deserialize,Clone,Debug)]
pub enum valid_type{
    // 验证成功
    success,
    // 验证失败 账号密码错误
    fail_passwordError,
    // 验证失败 账号不存在
    fail_accountNotFound,
}


impl ServerController {
    pub fn new() -> ServerController {
        let user = AccountCtl::new();
        let mut server = ServerCtl::new();
        let login = LoginSignCtl::new();
        Self {
            user,
            server,
            login,
        }
    }

    // 新建用户
    pub fn new_user(&mut self, login_info: &Login_Data) -> RegisterResult {
        return self.user.register_user(login_info.clone());
    }

    // 尝试使用签名登录
    pub fn try_sign_login(&mut self, login_data: &Login_Data, time:&i64, sign:&String) -> Result<(), SignLoginFailResult> {

        return self.login.try_sign_login(login_data, time, sign);
    }
    // 尝试使用签名登出
    pub fn try_sign_logout(&mut self, login_info: &Login_Data, time:&i64, sign:&String) -> bool {

        return self.login.try_sign_logout(login_info,time,sign);
    }

    // 验证用户是否存在
    pub fn valid_user(&mut self, account:String) -> bool {
        return self.user.check_user_exist(account);
    }

    // 获取用户信息
    pub fn get_user_info(&mut self, login_info:&Login_Data) -> Option<Player> {
        return self.user.get_user_record(login_info.account.clone());
    }

    // 验证账号和密码
    pub fn valid_user_account(&mut self,login_info:&Login_Data)->valid_type{
        // 判断账号和密码是否存在
        return if self.user.check_user_exist(login_info.account.clone()){
            // 判断账号和密码是否正确
            if let Some(player) = self.get_user_info(&login_info){
                if player.password==login_info.password {
                    valid_type::success
                }else{
                    valid_type::fail_passwordError
                }
            }else{
                valid_type::fail_passwordError
            }
        }else{
            // 密码不存在
            valid_type::fail_accountNotFound
        };
    }

    // 从账号获取密码信息
    pub fn get_password_from_account(&mut self, account:&String) -> Option<String> {
        return self.user.get_password_from_account(account);
    }

    // 获取一个空闲的专用服务器
    pub fn get_idle_dedicated_server(&mut self) -> Result<ServerAddr,&str> {
        let server=self.server.get_idle_server();
        if let Some(server_addr)=server {
            return Ok(server_addr);
        }
        return Err("没有空闲的专用服务器");
    }

    // 获取当前正在运行的专用服务器列表
    pub fn get_running_dedicated_server_list(&mut self) -> Vec<ServerAddr> {
        return self.server.get_all_server();
    }



    // 平衡服务器
    pub fn balance_dedicate_server(&mut self){

        // 平衡
        // @TODO: 需要新增服务器或者删除服务器的时候来进行平衡
        // 删除本地的服务器
        self.remove_timeout_server(ServerOwner::Local, 5);
        // 长时间保留玩家的房间，以等待玩家连接
        self.remove_timeout_server(ServerOwner::Player("".to_string()), 10);
        // 检查玩家的服务器
        self.remove_unrecord_server();


        let all_servers=self.get_running_dedicated_server_list();

        let mut empty_player_server=vec![];

        for server_addr in all_servers.iter(){
            if let Some(server_info) =self.get_server_info(server_addr){
                if(server_info.runtime_data.player_list.len() == 0){
                    empty_player_server.push(server_addr.clone());
                }
            }
        }

        if empty_player_server.len() != 3{
            if empty_player_server.len() >3 {
                // 减少服务器

                for server_addr in 0..empty_player_server.len()-3 {
                    
                    self.stop_server(empty_player_server[server_addr].clone());
                    
                }

            }else{
                // 增加服务器
                println!("新增服务器:{}",3-empty_player_server.len());
                self.extent_ds(3-empty_player_server.len())

            }
        }
    }
    // 删除没有记录在server_list当中的服务器
    pub fn remove_unrecord_server(&mut self){

        // 获取所有addr
        self.server.remove_unrecord_server();

    }
    // 用户加入到服务器
    pub fn join_server(&mut self, login_info: &Login_Data, server_addr: &ServerAddr) -> Result<(), &str> {
        let max_time=3000; // @TODO:这里应该是一个全局的参数
        if self.get_last_heartbeat_time(server_addr.clone()).unwrap_or(max_time) < max_time || true {
            // 服务器有效
            // 查看服务器的人数是否满员
            if let Some(mut server_info)=self.get_server_info(server_addr) {
                if server_info.runtime_data.player_count<server_info.runtime_data.max_player {

                    server_info.runtime_data.player_count+=1;
                    server_info.runtime_data.player_list.push(login_info.account.clone());
                    self.server.update_server_info(&server_addr.clone(),&server_info.clone());
                    // 加入服务器成功
                    //self.balance_dedicate_server();
                    return Ok(());
                }
                else { return Err("服务器人数已满") };
            }
            else {return Err("服务器不存在")};
        }
        return Err("没有空闲的专用服务器");
    }

    // 更新服务器心跳包
    pub fn update_server_heartbeat(&mut self, server_addr: &ServerAddr, x: &ServerHeartbeat) -> bool {
        // 检查服务器是否存在

        if let Some(info) = self.server.get_server_info(&server_addr){
            // 服务器存在

            //println!("服务器存在");
            
            match x.status {
                ServerRunningStatus::Running => {
                    // 继续更新心跳包
                    self.server.update_server_heartbeat(server_addr, x);

                }
                ServerRunningStatus::Stop => {
                    // 关闭服务器
                    self.server.stop_server(server_addr);
                }
                ServerRunningStatus::NoResponse => {
                    // 关闭服务器
                    self.server.stop_server(server_addr);
                }
            }
            return true;
        }
        //println!("服务器不存在");


        return false;
    }


    // 更新玩家的信息
    pub fn update_players(&mut self,server_addr:&ServerAddr,player_list:&Vec<String>)->Result<(), &str>{
        // 更新一个服务器里的信息
        return if let Some(mut info)=self.server.get_server_info(server_addr){
            info.runtime_data.player_list=player_list.clone();

            self.server.update_server_info(server_addr, &info);
            Ok(())

        }else {
            Err("not fund server")
        };
    }
    pub fn get_server_from_name(&mut self,server_name:String)->Option<ServerAddr>{
        return self.server.get_server_from_name(server_name);
    }

    // 获取超时的服务器
    pub fn get_timeout_servers(&mut self, timeout_secs: u64) -> Vec<ServerAddr> {
        // 删除超时的服务器
        let server_list=self.server.get_local_servers();
        let mut timeout_servers=vec![];
        for server_addr in server_list{
            if let Some(info)=self.server.get_server_info(&server_addr){

                let t=std::time::Duration::from_millis((chrono::Utc::now().timestamp_millis()-info.runtime_data.heartbeat.last_hart_beat_time) as u64);

                if t.as_secs()>timeout_secs{
                    timeout_servers.push(server_addr.clone());

                }
            }
        }
        return timeout_servers;
    }


    pub fn remove_timeout_server(&mut self, filter: ServerOwner, timeout_secs: u64){
        // 删除超时的服务器
        let mut timeout_servers =self.get_timeout_servers(timeout_secs).clone();
        let mut local_servers=self.server.server_ctl.get_local_servers();

        for timeout_server in timeout_servers.iter_mut() {
            for local_server in local_servers.iter_mut() {
                if local_server.id == timeout_server.server_name.parse::<u32>().unwrap_or_default() {
                    if let (x,y) =(local_server.owner.clone(),filter.clone()) {
                        match (x,y){
                            (ServerOwner::Player(_),ServerOwner::Player(_)) => {
                                self.server.stop_server(&timeout_server).expect("删除服务器失败");
                                break;
                            },
                            (ServerOwner::Local,ServerOwner::Local) => {
                                self.server.stop_server(&timeout_server).expect("删除服务器失败");
                                break;
                            },
                            (_,_) => {
                                // 类型不匹配 不删除
                            }
                        }
                    }
                }
            }
        }
    }
    // 移除所有服务器
    pub fn remove_all_server(&mut self){
        self.server.remove_all_server();
    }

    // 退出服务器
    pub fn quit_server(&mut self, login_info: &Login_Data, server_addr: &ServerAddr) -> Result<ServerAddr,&str> {
        // @TODO:退出服务器
        if let Some(mut data) = self.get_server_info(&server_addr) {
            // 查看服务器是否有该玩家
            if let Ok(index)=data.runtime_data.player_list.binary_search(&login_info.account.clone()){
                data.runtime_data.player_list.remove(index);
                data.runtime_data.player_count-=1;
                self.server.update_server_info(&server_addr.clone(),&data.clone());

                //self.balance_dedicate_server();

                return Ok(server_addr.clone());
            }else  {return Err("服务器没有找到该玩家登陆记录")}


        }else { return Err("服务器地址无效"); }

        return Err("还没有写这部分功能");
    }

    // 获取最后一次心跳包的时间
    pub fn get_last_heartbeat_time(&mut self, server_addr:ServerAddr) -> Option<i64> {
        // 查看最近的一次心跳包
        if let Some(info)=self.server.get_server_info(&server_addr){
            return Some(info.runtime_data.heartbeat.last_tick_time);
        }
        return None;
    }

    pub fn get_server_info(&mut self, server_addr: &ServerAddr) -> Option<ServerInfo> {

        return self.server.get_server_info(server_addr);
    }

    // 查看储存服务器是否存在
    pub fn valid_store_server(&mut self) -> RedisResult<Client> {
        return redis::Client::open(store_ctl::get_redis_path())
    }

    // 获取分数
    pub fn get_player_score(&mut self, account: String) -> Option<PlayerScore> {
        return self.user.get_player_score(account);

    }

    // 更新分数
    pub fn update_player_score(&mut self, account:String, score: PlayerScore) -> Result<(), &str> {
        return self.user.update_player_score(account, score);
    }

    // 添加新的服务器
    pub fn add_server(&mut self,server_addr: ServerAddr) -> Result<(), &str> {
        return self.server.add_server(server_addr);
    }

    // 终止一个服务器
    pub fn stop_server(&mut self, server_addr: ServerAddr) -> Result<(), &str> {
        return self.server.stop_server(&server_addr);
    }

    // 扩展专用服务器
    pub fn extent_ds(&mut self,extent_size:usize){
        self.server.extent_play_ds(extent_size);
    }

    // 扩展一个玩家专用服务器
    pub fn extent_player_ds(&mut self, account:String) -> u32 {
        return self.server.create_player_ds(account);
    }

    pub fn extent_login_ds(&mut self,extent_size:usize){
        self.server.extent_login_ds(extent_size);
    }



}

#[test]
fn lazy_test() {
    //SERVERCONTROLLER.valid_user("111".to_string());
    println!("{:?}",SERVERCONTROLLER.lock().unwrap().valid_user("test3".to_string()));
}