use crate::store::store_data::Login_Data;
use redis;
use redis::{Commands, RedisError};

pub struct data_ctl {
    // 数据库信息
    redis_ctl: redis::Connection,
    // 命名空间
    NAME_SPACE: String,
}


pub fn get_redis_path() -> &'static str {
    return "redis://azaz.ltd:6379";
}

impl data_ctl {
    pub fn new(name_space:&str) -> Self {
        let redis_path = get_redis_path();
        let redis_client = redis::Client::open(redis_path).unwrap();
        let redis_conn = redis_client.get_connection().unwrap();
        Self {
            redis_ctl: redis_conn,
            NAME_SPACE: name_space.to_string(),
        }
    }
    //查看记录是否存在
    pub fn check_record_exists(&mut self, key: &str) -> bool {
        let key = format!("{}:{}", self.NAME_SPACE, key);
        return self.redis_ctl.exists(key).unwrap();

    }
    // 更新数据 可以作为插入数据
    pub fn update_data(&mut self, key: &str, data: &str) {
        let key = format!("{}:{}", self.NAME_SPACE, key);
        let _: () = self.redis_ctl.set(key, data).unwrap();
    }
    // 删除数据
    pub fn delete_data(&mut self,key:&str){
        let key = format!("{}:{}", self.NAME_SPACE, key);

        let _: () = self.redis_ctl.del(key).unwrap();
    }
    pub fn delete_data_with_raw(&mut self,key:&str){
        let key=format!("{}",key);
        let _:()=self.redis_ctl.del(key).unwrap();
    }
    // 获取数据
    pub fn get_data(&mut self,key:&str) -> Option<String> {
        let key = format!("{}:{}", self.NAME_SPACE, key);
        let data = self.redis_ctl.get(key);
        return match data {
            Ok(data) => {
                Some(data)
            }
            Err(e) => {
                // println!("error:{}",e);
                None
            }
        }
    }
    // 使用通配符查询
    pub fn match_data(&mut self,con:String) -> Option<Vec<String>> {
        return if let Ok(Data) = self.redis_ctl.keys(format!("{}:{}",self.NAME_SPACE,con)){
            Some(Data)
        }else{
            None
        }
    }
    
}
