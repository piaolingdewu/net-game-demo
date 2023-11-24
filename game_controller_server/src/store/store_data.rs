use serde::{Deserialize, Serialize};

// 用户信息
#[derive(Serialize, Deserialize, Debug)]
pub struct Player {
    pub account: String,
    pub password: String,
    pub register_time: i64,
    pub last_login_time: i64,
    pub login_times: i64,
    pub score: PlayerScore,
}
#[derive(Serialize, Deserialize, Debug)]
pub struct PlayerScore {
    pub score: u32,
    pub rank: u32,
}



#[derive(Serialize, Deserialize, Debug,Clone)]
pub struct Login_Data{
    //登录名
    pub account: String,
    //登录密码
    pub password:String,
}

#[derive(Debug,Default,Serialize,Deserialize,Clone)]
pub struct ServerRuntime {
    pub heartbeat:ServerHeartbeat,
    pub create_time:i64,
    pub player_count:usize,
    pub max_player:usize,
    pub player_list:Vec<String>,
    pub round:usize,
    pub heart_beat_speed:i64,
}

#[derive(Debug,Serialize,Deserialize,Default,Clone)]
pub struct ServerAddr {
    pub ip:String,
    pub port:String,
    pub server_name:String,
}

impl PartialEq for ServerAddr {
    fn eq(&self, other: &Self) -> bool {
        self.ip == other.ip && self.port == other.port && self.server_name==other.server_name
    }
}



#[derive(Debug,Serialize,Deserialize,Clone,Default)]
pub enum  ServerRunningStatus {
    Running,
    #[default]
    Stop,
    // 无响应
    NoResponse,
}
#[derive(Debug,Serialize,Deserialize,Default,Clone)]
pub struct ServerHeartbeat{
    pub last_hart_beat_time:i64,
    pub last_10_ticks_average_time:i64,
    pub last_tick_time:i64,
    pub status:ServerRunningStatus,
}
#[derive(Debug,Serialize,Deserialize,Default,Clone)]
pub struct ServerInfo {
    // 服务器名称
    pub server_name:String,
    // 服务器地址
    pub addr: ServerAddr,
    // 服务器心跳信息
    pub runtime_data: ServerRuntime,
}



