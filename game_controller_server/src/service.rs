use actix_web::{get, post, web, App, HttpServer, Responder, HttpResponse, HttpRequest};
use actix_web::http::header::HeaderValue;
use actix_web::middleware::{Condition, Logger, NormalizePath};
use actix_web::web::route;
use serde::{Serialize,Deserialize};
use controller::SERVERCONTROLLER;
use crate::controller;
use crate::controller::valid_type;
use crate::controller::valid_type::{fail_accountNotFound, fail_passwordError, success};
use crate::store::store_data::{Login_Data, ServerAddr, ServerHeartbeat, ServerInfo, PlayerScore};
use crate::user::account_ctl::{AccountCtl, RegisterResult};
use crate::user::login_sign_ctl::SignLoginFailResult;


#[derive(Serialize, Deserialize, Debug,Default)]
struct ResponseData {
    success: bool,
    msg: String,
    data: serde_json::value::Value,
}

impl ResponseData {
    pub fn make_fail(msg: &str) -> ResponseData {
        ResponseData {
            success: false,
            msg: msg.to_string(),
            data: serde_json::value::Value::Null
        }
    }
    pub fn make_success(msg: &str, data:serde_json::value::Value) -> ResponseData {
        ResponseData {
            success: true,
            msg: msg.to_string(),
            data,
        }
    }

    pub fn to_json(&self)->String{
        serde_json::to_string(&self).unwrap().to_string()
    }
}

// 验证账号是否存在
#[get("/valid_account_exists")]
pub async fn valid_account_exists(http:HttpRequest) -> impl Responder {
    //从head里找到账号和密码
    let acc=http.headers().get("account").unwrap().to_str().unwrap_or_default();
    let pas=http.headers().get("password").unwrap().to_str().unwrap_or_default();
    println!("验证账户:{},{}",&acc,&pas);

    let account=Login_Data{
        account: acc.to_string(),
        password: pas.to_string()
    };
    if SERVERCONTROLLER.lock().unwrap().valid_user(account.account.clone()) {

        return HttpResponse::Ok().body(ResponseData::make_success("account is found",serde_json::value::Value::Null).to_json());
    }

    return HttpResponse::Ok().body(ResponseData::make_fail("account not found").to_json());

}

#[derive(Serialize,Deserialize,Clone,Debug)]
struct VaildResult {
    /*
        0: 成功
        1: 密码错误
        2: 账号不存在
     */
    result_code:u8,
    result_type:valid_type,
}

// 查看账号密码是否正确
#[get("/valid_account")]
pub async fn valid_account(http:HttpRequest) -> impl Responder {

    if let Ok(mut ctl) = SERVERCONTROLLER.lock(){
        let acc=http.headers().get("account").unwrap().to_str().unwrap_or_default();
        let pas=http.headers().get("password").unwrap().to_str().unwrap_or_default();

        println!("尝试验证账号:{:?}",&(acc,pas));
        let account=Login_Data{
            account: acc.to_string(),
            password: pas.to_string()
        };
        let mut  result= VaildResult {
            result_code:3,
            result_type:valid_type::fail_accountNotFound,
        };



        match ctl.valid_user_account(&account) {
            valid_type::success => {
                result.result_code=0;
                result.result_type=success;

            }
            valid_type::fail_passwordError => {
                result.result_code=1;
                result.result_type=fail_passwordError;
            }
            valid_type::fail_accountNotFound => {
                result.result_code=2;
                result.result_type=fail_accountNotFound;
            }
        }
        let data_string=serde_json::to_string(&result).unwrap();

        return HttpResponse::Ok().body(ResponseData::make_success(format!("{:?}",&result.result_type).as_str(),serde_json::from_str(&data_string).unwrap()).to_json());


        if ctl.valid_user(account.account.clone()) {
            if let Some(data) = ctl.get_user_info(&account){
                if data.password==account.password {

                    return HttpResponse::Ok().body(ResponseData::make_success("account and password is right",serde_json::value::Value::Null).to_json());
                }
            }
            return HttpResponse::Ok().body(ResponseData::make_fail("password is wrong").to_json());
        }
    }

    return HttpResponse::Ok().body(ResponseData::make_fail("account not found").to_json());

}


// 使用签名登录
#[get("/try_sign_login")]
pub async fn try_sign_login(http:HttpRequest) -> impl Responder {
    //从head里找到账号和密码
    let acc=http.headers().get("account").unwrap().to_str().unwrap_or_default();
    let time=http.headers().get("time").unwrap().to_str().unwrap_or_default();
    let sign=http.headers().get("sign").unwrap().to_str().unwrap_or_default();
    //println!("建立连接:{},{}",&acc,&pas);

    let mut ser_ctl=SERVERCONTROLLER.try_lock().unwrap();

    if let Some(passwd) =ser_ctl.get_password_from_account(&acc.to_string()) {

        let login_data=Login_Data{
            account:acc.to_string(),
            password:passwd.to_string(),
        };
        match ser_ctl.try_sign_login(&login_data,&time.parse::<i64>().unwrap(),&sign.to_string()) {
            Ok(_) => {
                return HttpResponse::Ok().body(ResponseData::make_success("login finish",serde_json::value::Value::Null).to_json());
            }
            Err(err) => {
                match err {
                  SignLoginFailResult::FailSignInvalid => {

                      return HttpResponse::Ok().body(ResponseData::make_fail("FailSignInvalid").to_json());
                  }
                  SignLoginFailResult::FailSignExist => {
                      return HttpResponse::Ok().body(ResponseData::make_fail("FailSignExist").to_json());
                  }
                  SignLoginFailResult::FailSignTimeout => {
                      return HttpResponse::Ok().body(ResponseData::make_fail("FailSignTimeout").to_json());
                  }
                }
            }
        }

    }else {
        return HttpResponse::Ok().body(ResponseData::make_fail("account not found").to_json());
    }

}

// 使用签名退出
#[get("/try_sign_logout")]
pub async fn try_sign_logout(http:HttpRequest) -> impl Responder {
    //从head里找到账号和密码
    let acc=http.headers().get("account").unwrap().to_str().unwrap_or_default();
    let time=http.headers().get("time").unwrap().to_str().unwrap_or_default();
    let sign=http.headers().get("sign").unwrap().to_str().unwrap_or_default();
    //println!("建立连接:{},{}",&acc,&pas);

    let mut ser_ctl=SERVERCONTROLLER.try_lock().unwrap();

    if let Some(passwd) =ser_ctl.get_password_from_account(&acc.to_string()) {
        let login_data = Login_Data {
            account: acc.to_string(),
            password: passwd.to_string(),
        };
        return if ser_ctl.try_sign_logout(&login_data, &time.parse::<i64>().unwrap(), &sign.to_string()) {
            HttpResponse::Ok().body(ResponseData::make_success("logout success",serde_json::value::Value::Null).to_json())

        }else {
            HttpResponse::Ok().body(ResponseData::make_fail("logout fail").to_json())

        }
    }
    return HttpResponse::Ok().body(ResponseData::make_fail("account not found").to_json())
}



// 获取账号的信息
#[get("/get_account_info")]
pub(crate) async fn get_account_info(http:HttpRequest) -> impl Responder {
    //从head里找到账号和密码
    let acc=http.headers().get("account").unwrap().to_str().unwrap_or_default();
    let pas=http.headers().get("password").unwrap().to_str().unwrap_or_default();
    let login_data =Login_Data{ account: acc.to_string(), password: pas.to_string() };
    println!("建立连接:{:?}",&login_data);

    if let Some(data)=SERVERCONTROLLER.lock().unwrap().get_user_info(&login_data) {
        if pas.to_string()!=data.password {
            return HttpResponse::Ok().body(ResponseData::make_fail("password is wrong").to_json());
        }
        let out_data=serde_json::to_string(&data).unwrap();

        return HttpResponse::Ok().body(ResponseData::make_success("account is found",serde_json::from_str(&out_data).unwrap()).to_json());
    }

    return HttpResponse::Ok().body(ResponseData::make_fail("account not found").to_json());
}

// 获取所有服务器信息
#[get("/get_all_servers")]
pub(crate) async fn get_all_servers() -> impl Responder {
    if let Ok(mut ctl) = SERVERCONTROLLER.lock() {
        if let server_list=ctl.get_running_dedicated_server_list(){

            // 删除所有的超时服务器
            //ctl.remove_timeout_server();
            let out_data=serde_json::to_string(&server_list).unwrap();


            return HttpResponse::Ok().body(ResponseData::make_success("",serde_json::from_str(&out_data).unwrap()).to_json());
        }
    };

    return HttpResponse::Ok().body(ResponseData::make_fail("not found dedicate server").to_json());
}

// 获取所有服务器信息
#[get("/get_all_servers_info")]
pub(crate) async fn get_all_servers_info() -> impl Responder {

    if let Ok(mut ctl) = SERVERCONTROLLER.lock() {
        if let server_list=ctl.get_running_dedicated_server_list(){

            let mut out_info=vec![];
            // 删除所有的超时服务器
            //ctl.remove_timeout_server();

            for serverAddr in server_list {

                if let Some(server_info) = ctl.get_server_info(&serverAddr) {
                    out_info.push(server_info);
                }

            }

            let string=serde_json::to_string(&out_info).unwrap();

            return HttpResponse::Ok().body(ResponseData::make_success("",serde_json::from_str(&string).unwrap()).to_json());
        }
    }
    return HttpResponse::Ok().body(ResponseData::make_fail("not found any dedicate server").to_json());
}


// 获取一个服务器信息
#[get("/get_server_info")]
pub(crate) async fn get_server_info(request:HttpRequest) -> impl Responder {

    let host=request.headers().get("host").unwrap().to_str().unwrap_or_default();
    let port=request.headers().get("port").unwrap().to_str().unwrap_or_default();
    //let name=request.headers().get("name").unwrap().to_str().unwrap_or_default();

    let server_addr =ServerAddr{
        ip: host.to_string(),
        port: port.to_string(),
        server_name: "".to_string(),
    };

    return match &SERVERCONTROLLER.lock().unwrap().get_server_info(&server_addr){
        None => {
            HttpResponse::Ok().body(ResponseData::make_fail("not found server").to_json())
        }
        Some(server_info) => {
            let out_data=serde_json::to_string(&server_info).unwrap();

            HttpResponse::Ok().body(ResponseData::make_success("",serde_json::from_str(&out_data).unwrap()).to_json())

        }
    }
}


// 获取一个空闲的服务器
#[get("/get_free_server")]
pub(crate) async fn get_free_server() -> impl Responder {
    return match &SERVERCONTROLLER.lock().unwrap().get_idle_dedicated_server(){
        Ok(addr) => {
            let out_data=serde_json::to_string(&addr).unwrap();

            HttpResponse::Ok().body(ResponseData::make_success("",serde_json::from_str(&out_data).unwrap()).to_json())
        }
        Err(err) => {
            HttpResponse::Ok().body(ResponseData::make_fail(&err.to_string()).to_json())
        }
    }
}

// 注册一个用户
#[post("/register_account")]
pub async fn register(account: web::Json<Login_Data>) -> impl Responder {
    let result=SERVERCONTROLLER.lock().unwrap().new_user(&account.0);
    return match result {
        RegisterResult::Success => {
            HttpResponse::Ok().body(ResponseData::make_success("register success",serde_json::value::Value::Null).to_json())
        }
        RegisterResult::FailExisted => {
            HttpResponse::Ok().body(ResponseData::make_fail("account is existed").to_json())
        }
    }
}



#[derive(Serialize,Deserialize)]
pub struct LoginInfo {
    login:Login_Data,
    server_add:ServerAddr,
}

// 用户加入到服务器
#[post("/join_server")]
pub async fn join_server(body :web::Json<LoginInfo>) -> impl Responder {


    return match(SERVERCONTROLLER.lock()){
        Ok(mut ctl) =>{
            match(ctl.join_server(&body.login, &body.server_add)){
                Ok(())=>{
                    println!("用户加入进服务器");
                    // 查看是需要增加还是减少服务器
                    ctl.balance_dedicate_server();
                        
                    HttpResponse::Ok().body(ResponseData::make_success("join server success",serde_json::value::Value::Null).to_json())

        
                }Err(err)=>{
                    HttpResponse::Ok().body(ResponseData::make_fail(&err.to_string()).to_json())

                }
            }


            
        }Err(err)=>{
            HttpResponse::Ok().body(ResponseData::make_fail(&err.to_string()).to_json())

        }
    }
}

// 用户退出服务器
#[post("/quit_server")]
pub async fn quit_server(body:web::Json<LoginInfo>) -> impl Responder {
    return match SERVERCONTROLLER.lock().unwrap().quit_server(&body.login,&body.server_add) {
        Ok(_) => {
            HttpResponse::Ok().body(ResponseData::make_success("quit server success",serde_json::value::Value::Null).to_json())
        }
        Err(err) => {
            HttpResponse::Ok().body(ResponseData::make_fail(&err.to_string()).to_json())
        }
    };
}


#[derive(Serialize,Deserialize)]
pub struct Players_info{
    
    // 主机地址
    server_add:ServerAddr,
    // 账号列表
    account_list:Vec<String>
}


// 更新一个地图里有多少个玩家
#[post("/update_players")]
pub async fn update_players(body:web::Json<Players_info>) -> impl Responder{

    // 更新玩家
    //println!("update players:{},{:?}",&body.server_add.server_name,&body.account_list);

    return match SERVERCONTROLLER.lock() {
        Ok(mut ctl) => {
            ctl.update_players(&body.server_add,&body.account_list);

            // 平衡服务器
            //ctl.balance_dedicate_server();
            HttpResponse::Ok().body(ResponseData::make_success("update players success",serde_json::value::Value::Null).to_json())
        }
        Err(err) => {
            HttpResponse::Ok().body(ResponseData::make_fail(&err.to_string()).to_json())
        }
    };
}

#[derive(Serialize,Deserialize,Debug)]
pub struct UpdateHeartbeat {
    pub server_addr:ServerAddr,
    pub server_heartbeat:ServerHeartbeat,

}

// 更新心跳包
#[post("/heartbeat")]
pub async fn heartbeat(server_heartbeat:web::Json<UpdateHeartbeat>) -> impl Responder{
    //println!("{:?}",server_heartbeat.0);
    //println!("心跳包:{:?}",&server_heartbeat.0);


    // 更新服务器的状态
    //println!("addr:{:?}",&server_heartbeat.server_addr);
    //let head_name=http_request.headers().get("HostName").unwrap_or("None".to_string()).to_str();
    
    //println!("心跳包时间:{}",server_heartbeat.server_heartbeat.last_hart_beat_time);
    
    
    return match SERVERCONTROLLER.lock(){
        Ok(mut ctl) => {
            // 判断名字是否不对
            //ctl.get_server_info(s)
            if ctl.update_server_heartbeat(&server_heartbeat.server_addr,&server_heartbeat.server_heartbeat){
                // println!("心跳包更新成功");
                //println!("更新成功");
                HttpResponse::Ok().body(ResponseData::make_success("update heartbeat success",serde_json::value::Value::Null).to_json())

                // 更新成功
            }else{
                // 更新失败 这里更新失败的时候 查询是否有这个服务器 如果没有就添加这个服务器
                let mut server_addr=server_heartbeat.server_addr.clone();
                //server_addr.server_name=server_name.to_string();
                
                //println!("添加服务器:{}",&server_addr.server_name);
                if let Ok(server) = ctl.add_server(server_addr){
                    return if ctl.update_server_heartbeat(&server_heartbeat.server_addr,&server_heartbeat.server_heartbeat) {
                        HttpResponse::Ok().body(ResponseData::make_success("update heartbeat success",serde_json::value::Value::Null).to_json())
                        
                    }else{HttpResponse::Ok().body(ResponseData::make_fail("update heartbeat fail unknow fail type").to_json())}
                }else{
                    HttpResponse::Ok().body(ResponseData::make_fail("update heartbeat fail unknow fail type").to_json())
                }
            }
        },
        Err(err) => {
            HttpResponse::Ok().body(ResponseData::make_fail("update heartbeat fail maybe server offline").to_json())
        }
    }
}

// 获取用户分数
#[get("/get_player_score")]
pub async fn get_player_score(request:HttpRequest) -> impl Responder{

    if let Some(account) = request.headers().get("account") {
        match SERVERCONTROLLER.lock().unwrap().get_player_score(account.to_str().unwrap_or_default().to_string()) {
            None => {
                HttpResponse::Ok().body(ResponseData::make_fail("account not found").to_json())
            }
            Some(score) => {
                let out_data=serde_json::to_string(&score).unwrap();
                HttpResponse::Ok().body(ResponseData::make_success("get player score success",serde_json::from_str(&out_data).unwrap()).to_json())
            }
        }
    }
    else { HttpResponse::Ok().body(ResponseData::make_fail("account not found").to_json()) }
}
// 更新玩家分数
#[post("/update_player_score")]
pub async fn update_player_score(request:HttpRequest, body:web::Json<PlayerScore>) ->impl Responder{

    // 是否要验证账号呢?
    let account=request.headers().get("account").unwrap().to_str().unwrap_or_default();
    //println!("{:?}",&body.0);

    let score=body.0;


    return match SERVERCONTROLLER.lock().unwrap().update_player_score(account.to_string(),score){
        Ok(_) => {
            HttpResponse::Ok().body(ResponseData::make_success("update player score success",serde_json::value::Value::Null).to_json())
        }
        Err(err) => {
            HttpResponse::Ok().body(ResponseData::make_fail(&err.to_string()).to_json())
        }
    };
}


// 添加一个服务器
#[post("/add_server")]
pub async fn add_server(request:HttpRequest,server_info:web::Json<ServerAddr>) -> impl Responder {

    let create_time=chrono::Utc::now().timestamp_micros();

    let server_name=if let Some(name) = request.headers().get("ServerName"){
        name.to_str().unwrap_or_default().to_string()

    }else{format!("defaul_name_{}",create_time.to_string())};

    return match SERVERCONTROLLER.lock().unwrap().add_server(server_info.0){
        Ok(_) => {
            HttpResponse::Ok().body(ResponseData::make_success("add server success",serde_json::value::Value::Null).to_json())
        }
        Err(err) => {
            HttpResponse::Ok().body(ResponseData::make_fail(&err.to_string()).to_json())
        }
    };
}
// 销毁一个服务器
#[post("/stop_server")]
pub async fn stop_server(server_info:web::Json<ServerAddr>) -> impl Responder {
    //println!("销毁服务器:{:?}",server_info.0);
    return match SERVERCONTROLLER.lock().unwrap().stop_server(server_info.0){
        Ok(_) => {
            HttpResponse::Ok().body(ResponseData::make_success("stop server success",serde_json::value::Value::Null).to_json())
        }
        Err(err) => {
            HttpResponse::Ok().body(ResponseData::make_fail(&err.to_string()).to_json())
        }
    };
}


#[get("/get_server_from_name")]
pub async fn get_server_from_name(request:HttpRequest) -> impl Responder {
    let server_name=request.headers().get("ServerName").unwrap().to_str().unwrap_or_default();
    if server_name.is_empty() {
        return HttpResponse::Ok().body(ResponseData::make_fail("not ServerName in headers").to_json())

    }
    return match SERVERCONTROLLER.lock().unwrap().get_server_from_name(server_name.to_string()){
        Some(server) => {
            let out_data=serde_json::to_string(&server).unwrap();
            HttpResponse::Ok().body(ResponseData::make_success("get server success",serde_json::from_str(&out_data).unwrap()).to_json())
        }
        None => {
            HttpResponse::Ok().body(ResponseData::make_fail("Not target server").to_json())
        }
    }
}

// 创建指定玩家房间
#[post("/create_room")]
pub async fn create_room(request:HttpRequest) -> impl Responder {
    // 获取用户名

    let account=request.headers().get("Account").unwrap().to_str().unwrap_or_default();

    //println!("销毁服务器:{:?}",server_info.0);
    return match SERVERCONTROLLER.lock(){
        Ok(mut ctl) => {
            return if !account.is_empty() {
                let server_id=ctl.extent_player_ds(account.to_string());
                println!("创建玩家房间:{},{}",&account,&server_id);
                let mut server_addr=ServerAddr::default();
                server_addr.ip="*".to_string();
                server_addr.port="*".to_string();

                server_addr.server_name=server_id.to_string();
                let body_string=serde_json::to_string(&server_addr).unwrap_or_default();

                HttpResponse::Ok().body(ResponseData::make_success("create success",serde_json::from_str(body_string.as_str()).unwrap_or_default()).to_json())

            }else{
                HttpResponse::Ok().body(ResponseData::make_fail("Account is empty").to_json())

            }
        }
        Err(err) => {
            HttpResponse::Ok().body(ResponseData::make_fail(&err.to_string()).to_json())
        }
    };
}