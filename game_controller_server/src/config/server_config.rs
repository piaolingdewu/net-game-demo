
use serde::{Serialize,Deserialize};
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct ServerConfig{
    // 单个地图最大的玩家数
    max_player:u32,
    // 单个地图的Tick值 默认是60
    tick:u32,
    // 服务器缓冲数量
    server_buffer:u32,
}

// 载入


