use std::io::{BufRead, Read};
use std::process::{Child, Output};
use std::sync::{Arc, Mutex};
use chrono::format::Pad::Space;
use crate::store::store_ctl::data_ctl;
use serde::{Deserialize, Serialize};
use crate::store::store_data::{ServerAddr, ServerHeartbeat, ServerInfo, ServerRunningStatus, ServerRuntime};

pub struct ServerCtl {
    pub store_ctl:data_ctl,
    pub server_ctl:ServerRunner,
    
    // 登录服务器名称
    login_map_name:String,
    // 游戏服务器名称
    play_map_name:String,
}


#[derive(Debug,Clone,PartialOrd, PartialEq)]
pub enum ServerOwner {
    // 本地创建的房间，本地动态平衡玩家的数量
    Local,
    // 为指定玩家创建的房间，当该玩家离开时房间被销毁
    Player(String),
}

// 服务器运行信息
#[derive(Debug,Clone)]
pub struct ServerRunnerData{
    // 名称
    pub id: u32,
    // 服务器名称
    pub name:String,
    // 运行的实例
    pub process:Arc<Mutex<std::process::Child>>,
    // 是否正在工作
    pub status:ServerRunningStatus,
    // 所有者 [Player,Remote,Local]
    pub owner:ServerOwner,
}
pub struct ServerRunner {
    // 服务器的文件所在位置
    pub server_path:String,
    // 已经部署的服务器列表
    pub server_list:Vec<ServerRunnerData>
}

impl ServerRunner {
}


impl ServerRunner {
    fn new(server_path:String) -> Self {
        Self{
            server_path,
            server_list: vec![],
        }
    }

}

impl ServerRunner{
    // 尝试扩展新服务器
    pub fn extent_server(&mut self, map_name:String, extent_size:usize) -> Vec<usize> {

        let mut remain_count=extent_size;

        let mut out_list=vec![];
        while remain_count > 0 {
            let name=chrono::Utc::now().to_string();
            if let Some(process) = self.create_server(name.clone(), map_name.clone() ) {
                let id=process.id();
                let server:ServerRunnerData=ServerRunnerData{
                    id,
                    name,
                    process:Arc::new(Mutex::new(process)),
                    status: ServerRunningStatus::Running,
                    owner:ServerOwner::Local,
                };
                self.server_list.push(server);
                remain_count-=1;
                out_list.push(self.server_list.len()-1);
            }
        }
        return out_list;
    }



    // 创建玩家服务器
    pub fn extent_player_server(&mut self, map_name:String, account:String) -> u32 {
        let server_index=self.extent_server(map_name,1);

        self.server_list[server_index[0]].owner=ServerOwner::Player(account);

        return self.server_list[server_index[0]].id;
    }

    // 创建服务器列表
    pub fn get_local_servers(&mut self) -> Vec<ServerRunnerData> {
        return self.server_list.clone();
    }


    // 创建服务器
    fn create_server(&self, server_name: String, map_name:String) -> Option<Child> {

        // 休眠几毫秒再执行 避免重复执行的名字导致错位
        let server_name=server_name.replace(" ", "_");

        //let ctl_server=&format!("{} ServerName {} LOG={}.txt ControllerServer 127.0.0.1:7001",map_name,&server_name,&server_name);

        let server_map=map_name.clone();
        let server_log=format!("LOG={}.txt",server_name);
        let server_ctl=format!("ControllerServer 127.0.0.1:7001");
        let server_sname=format!("ServerName {}",server_name);
        
        let mut args=vec![];
        args.push(&server_map);
        args.push(&server_log);
        args.push(&server_ctl);
        args.push(&server_sname);

        let arg=format!("{} LOG={}.txt ControllerServer 127.0.0.1:7001 ServerName {}",map_name,server_name,"cccc");
        match std::process::Command::new(&self.server_path)
            .args(&args)
            //.arg(&arg)
            .stdout(std::process::Stdio::null())
            .stderr(std::process::Stdio::null())
            .spawn()
        {
            Ok(child) => {
                println!("{}",format!("create ds finish:{:?}",&args));
                Some(child)
            } Err(err) => {
                println!("create server fail:{:?}",err);
                None
            }
        }
        //休眠
        
    }

    // 保留符合条件的服务器
    pub fn retain_server(&mut self,pred:fn(&ServerRunnerData)->bool){
        //
        for server in &mut self.server_list {

            if pred(server) {

                server.process.try_lock().unwrap().kill().unwrap_or_else(|err|{
                    println!("destroy server fail:{}",err);

                });
            }
        }
    }


    // 销毁服务器
    fn kill_server_from_id(&mut self, id:u32){
        for mut server in self.server_list.iter_mut() {
            if server.id==id{
                server.process.try_lock().unwrap().kill().unwrap();
            }
        }
        self.server_list.retain(|server| {
            if server.id == id {
                return false;
            }
            return true;
        });
    }


}



impl ServerCtl {
    pub fn new() -> ServerCtl {
        let space_name="server";
        let store_ctl=data_ctl::new(space_name);
        let server_path=format!("{}/LinuxServer/net_game_demo/Binaries/Linux/demoServer",std::env::var("HOME").unwrap_or_default());
        //let server_path=format!("{}/LinuxServer/demoServer.sh",std::env::var("HOME").unwrap_or_default());
        let ctl=ServerRunner{
            server_path,
            server_list: vec![],
        };

        Self{
            store_ctl,
            server_ctl:ctl,
            login_map_name: "Login_World_Map".to_string(),
            play_map_name: "Play_World_Map".to_string(),
        }
    }
    pub fn get_server_from_name(&mut self, server_name: String) -> Option<ServerAddr> {
        let server_list=self.get_all_server();

        for server in server_list {
            if server.server_name==server_name {
                return Some(server);
            }
        }
        return None;
    }


    // 更新服务器信息这实际上应该是心跳包
    pub fn update_server_info(&mut self, addr: &ServerAddr, info: &ServerInfo){
        self.store_ctl.update_data(format!("server_addr:{:?}",addr).as_str(),serde_json::to_string(&info).unwrap().as_str());
    }

    // 删除没有记录的服务器
    pub fn remove_unrecord_server(&mut self){

        let con="server_addr:*";
        let all_server=self.get_all_server();

        if let Some(data_string) = self.store_ctl.match_data(con.to_string()){
            //println!("data_list:{:?}",&data_string);

            for item in data_string  {
                // @TODO:修改为ServerAddr
                if let Ok(server)=serde_json::from_str::<ServerAddr>(&item){
                    println!("remove :{}",format!("server_addr:{:?}",server).as_str());

                    // 查询是否存在
                    if !all_server.contains(&server) {
                        
                        // 如果不存在 则删除
                        self.store_ctl.delete_data(format!("server_addr:{:?}",server).as_str());
                    }
                    
                }
            }
        }
        
    }
    // 删除所有服务器
    pub fn remove_all_server(&mut self){
        
        //let all_server=self.get_all_server();

        let match_con="server_addr:*";

        // 删除服务器信息
        if let Some(addr_list) =self.store_ctl.match_data(match_con.to_string()){
            for item in addr_list{
                //println!("search_result:{}",item);
                self.store_ctl.delete_data_with_raw(&item);
            }
        }
        // 删除服务器列表记录
        
        self.store_ctl.delete_data(format!("server_list").as_str());

    }
    // 删除服务器

    pub(crate) fn update_server_heartbeat(&mut self, server_addr: &ServerAddr, heartbeat: &ServerHeartbeat) -> bool {
        
        if let Some(mut data)=self.get_server_info(server_addr){
            data.runtime_data.heartbeat= heartbeat.clone();
            self.update_server_info(server_addr, &data);
            return true;
        }
        return false;
    }

    // 扩展默认数量的专用服务器
    pub fn extent_play_ds(&mut self,extent_size:usize){
        // 默认数量的服务器
        self.server_ctl.extent_server(self.play_map_name.clone(),extent_size);
    }
    // 创建玩家自己的房间
    pub fn create_player_ds(&mut self, account:String) -> u32 {
        return self.server_ctl.extent_player_server(self.play_map_name.clone(),account);
    }

    // 扩展默认数量的专用服务器
    pub fn extent_login_ds(&mut self,extent_size:usize){
        // 默认数量的服务器
        self.server_ctl.extent_server(self.login_map_name.clone(),extent_size);
    }


    // 新增服务器 这一段请求应该是有专用服务器发出的
    pub fn add_server(&mut self,addr: ServerAddr) -> Result<(), &str> {
        // 查看是否有重复的服务器
        let server_list=self.get_all_server();
        for server in server_list.iter(){
            if server.ip==addr.ip && server.port==addr.port{
                return Err("has this server");
            }
        }
        //println!("serveradd-aaaa:{:?}",&addr);

        let default_info= ServerInfo {
            addr:addr.clone(),
            runtime_data: ServerRuntime {
                heartbeat: Default::default(),
                create_time:chrono::Utc::now().timestamp_millis(),
                player_count:0,
                max_player: 3, //@TODO: 应该有一个全局配置来设置服务器的大小
                player_list:vec![],
                round:0,
                heart_beat_speed:0,
            },
            server_name: addr.clone().server_name,
        };

        self.store_ctl.update_data(format!("server_addr:{:?}",addr).as_str(),serde_json::to_string(&default_info).unwrap().as_str());


        // 更新服务器列表
        let mut new_server_list=self.get_all_server();
        //println!("add server :{:?}",&new_server_list);

        new_server_list.push(addr);
        
        self.store_ctl.update_data("server_list",serde_json::to_string(&new_server_list).unwrap().as_str());

        return Ok(());
    }

    // 停用服务器 这一段请求也应该是专用服务器发出的
    pub fn stop_server(&mut self, addr: &ServerAddr) -> Result<(),&str> {

        // @TODO: 目前考虑机制，当专用服务器 发送十秒的心跳包被连续拒收，那么专用服务器就自己销毁。
        println!("移除服务器:{}",format!("server_addr:{:?}",addr).as_str());
        let mut new_server_list=self.get_all_server();
        let mut has_server=false;
        new_server_list.retain(|x| {
            let result=(x.ip==addr.ip && x.port==addr.port);
            if result{
                has_server=true;
            }
            return !result;
        });

        //println!(format!("server_addr:{:?}", &addr);

        return if has_server {
            // 成功删除了指定的服务器
            self.server_ctl.kill_server_from_id(addr.server_name.parse::<u32>().unwrap());
            self.store_ctl.update_data("server_list", serde_json::to_string(&new_server_list).unwrap().as_str());
            self.store_ctl.delete_data(format!("server_addr:{:?}", addr).as_str());
            Ok(())
        } else { Err("not found server in server list") }
    }

    // 获取现在正在工作的所有服务器
    pub fn get_all_server(&mut self) -> Vec<ServerAddr> {
        if let Some(data)=self.store_ctl.get_data("server_list"){
            let server_list:Vec<ServerAddr>=serde_json::from_str(data.as_str()).unwrap();
            return server_list;
        }else {
            self.store_ctl.update_data("server_list","[]");
        }
        return vec![];
    }

    // 获取本地的服务器列表
    pub fn get_local_servers(&mut self) -> Vec<ServerAddr> {
        let all_server=self.get_all_server();
        let mut local_servers =vec![];
        // @TODO:这里可以提前记录 用于优化当前的双循环
        for server in all_server {
            for server_data in self.server_ctl.server_list.iter(){
                if server.server_name==server_data.name{
                    local_servers.push(server.clone());
                    break;
                }
            }

        }
        return local_servers;
    }

    // 获取指定服务器的信息
    pub fn get_server_info(&mut self, addr: &ServerAddr) -> Option<ServerInfo>{
        //println!("{}",format!("server_addr:{}:{}",&addr.ip,&addr.port ));
        

        if let Some(data)= self.store_ctl.get_data(format!("server_addr:{:?}",addr).as_str()) {


            if let Ok(server_info) = serde_json::from_str::<ServerInfo>(data.as_str()){
                return Some(server_info);
            }
            return None
        };
        None
    }

    // 获取空闲的服务器
    pub fn get_idle_server(&mut self) -> Option<ServerAddr> {
        let server_list=self.get_all_server();
        for server in server_list {
            if let Some(data)= self.store_ctl.get_data(format!("server_addr:{:?}",server).as_str()) {

                let server_info:ServerInfo=serde_json::from_str(data.as_str()).unwrap();
                if server_info.runtime_data.player_count<server_info.runtime_data.max_player {
                    return Some(server);
                }
            }
        }
        None
    }

}


use rand::Rng;
use rand::distributions::Uniform;
use rand::distributions::Distribution;
use crate::config;
use crate::service::add_server;
use crate::store::store_data::ServerRunningStatus::Running;
use crate::user::account_ctl::AccountCtl;

fn generate_random_ip() -> String {
    let mut rng = rand::thread_rng();
    let octet_dist = Uniform::new_inclusive(0, 255); // IP 地址每个 octet 的取值范围是 0 到 255
    let octets: Vec<u8> = (0..4).map(|_| octet_dist.sample(&mut rng)).collect(); // 生成4 个随机 octet 数组
    octets.iter().map(|o| o.to_string()).collect::<Vec<String>>().join(".") // 将每个 octet 转换成字符串，并使用 "." 连接成 IP 地址
}






