use md5::Digest;
use serde::{Deserialize, Serialize};
use crate::store::store_data::{Login_Data, Player, PlayerScore};
use crate::store::store_ctl::data_ctl;
struct UserName {
    name:String

}

pub struct AccountCtl {
    store_ctl:data_ctl
}



#[derive(Debug,Serialize,Deserialize)]
pub struct UserInfo {
    last_register_time:String,
    last_register_count:usize,
}

#[derive(Debug,Serialize,Deserialize)]
pub enum RegisterResult{
    Success,
    // 已经有存在的
    FailExisted,
}

#[derive(Debug,Serialize,Deserialize)]
pub enum ValidAccount {
    // 账号不存在
    NotExist,
    // 密码错误
    PasswordError,
    // 验证成功
    Success,
}

impl AccountCtl {
    pub fn new() -> AccountCtl {
        let space_name="user";
        let store_ctl=data_ctl::new(space_name);
        Self{
            store_ctl,
        }
    }

    // 注册用户
    pub fn register_user(&mut self, login_data:Login_Data) -> RegisterResult {
        // 检查用户名是否重复

        if !self.check_user_exist(login_data.account.clone()) {

            // 用户不存在
            //可以注册
            // 插入注册用户数据
            let user_record= Player {
                account: login_data.account.clone().to_string(),
                password: login_data.password.clone().to_string(),
                register_time: chrono::Utc::now().timestamp_micros(),
                last_login_time: chrono::Utc::now().timestamp_micros(),
                login_times: 0,
                score: PlayerScore { score: 0, rank: 0 },
            };

            self.update_player_data(login_data.account.clone(), user_record);
            return RegisterResult::Success;

        }else {
            // 用户已存在
            // 不可以注册
            // 返回错误
            return RegisterResult::FailExisted;
        }
    }


    pub(crate) fn get_password_from_account(&mut self, acc:&String) -> Option<String> {

        return if let Some(data) = self.store_ctl.get_data(format!("account:{}", acc).as_str()) {

            let user_record: Player =serde_json::from_str(data.as_str()).unwrap();
            Some(user_record.password)


        }else {
            None
        }
    }

    // 更新用户数据
    pub fn update_player_data(&mut self, account:String, user_record: Player){
        return self.store_ctl.update_data(format!("account:{}",account).as_str(),serde_json::to_string(&user_record).unwrap().as_str());
    }

    // 更新玩家分数
    pub fn update_player_score(&mut self, account:String, score: PlayerScore) -> Result<(),&str> {
        if let Some(mut user_record)=self.get_user_record(account.clone()){
            user_record.score=score;
            self.update_player_data(account, user_record);
            return Ok(());
        }
        return Err("not player account!");
    }

    // 获取用户信息
    pub fn get_user_record(&mut self, account: String) -> Option<Player> {
        return if let Some(data) = self.store_ctl.get_data(format!("account:{}", account).as_str()){
            let user_record: Player =serde_json::from_str(data.as_str()).unwrap();
            Some(user_record)
        }else {
            None
        }
    }
    // 验证玩家账号和密码是否正确
    pub fn valid_account_and_password(&mut self, login_Data:Login_Data) -> ValidAccount {
        // 登录数据
        if let Some(accountData)=self.store_ctl.get_data(format!("account:{}",login_Data.account).as_str()){
            if let Ok(user_record) =serde_json::from_str::<Player>(accountData.as_str()){
                if user_record.password==login_Data.password{
                    // 密码正确
                    // 返回成功
                    return ValidAccount::Success;

                }else {
                    // 密码错误
                    // 返回失败
                    return ValidAccount::PasswordError;
                }
            }
        }
        return ValidAccount::NotExist;
    }


    // 验证用户是否存在
    pub fn check_user_exist(&mut self, account: String) -> bool {

        return self.store_ctl.check_record_exists(format!("account:{}",account).as_str());
    }


    // 获取分数
    pub fn get_player_score(&mut self, account: String) -> Option<PlayerScore> {
        let data=self.store_ctl.get_data(format!("account:{}",account).as_str());
        if let Some(data) = data {
            let user_record: Player = serde_json::from_str(data.as_str()).unwrap();
            return Some(user_record.score);
        }
        return None;
    }
}

#[test]
fn test_func(){
    //生成随机数量的账号
    let mut account_ctl=AccountCtl::new();

    let mut account_list:Vec<String>=Vec::new();
    for index in 0..7 {
        let user_name=format!("test{}",index);
        account_ctl.register_user(Login_Data{
            account: user_name.clone(),
            password: "test".to_string(),
        });
        account_list.push(user_name);
    }

    println!("账号是否有效:{}:{}",account_list[3],account_ctl.check_user_exist(account_list[3].clone()));
    println!("账号是否有效:{}:{}","this_is_a_normal_test",account_ctl.check_user_exist("this_is_a_normal_test".to_string()));


    // 更新账号数据
    account_ctl.update_player_data(account_list[3].clone( ), Player {
        account: account_list[3].clone(),
        password: "test".to_string(),
        register_time: chrono::Utc::now().timestamp_micros(),
        last_login_time: chrono::Utc::now().timestamp_micros(),
        login_times: 0,
        score: PlayerScore { score: 5, rank: 3 },
    });
}


#[test]
pub fn get_account_hash(){
    let row_data=format!("{}:{}:{}","12311","123123",chrono::Utc::now().timestamp_micros());

    let sign=md5::compute(row_data.as_bytes());

    println!("{:X}",sign);
}

#[test]
pub fn get_password(){

    let mut account_ctl=AccountCtl::new();

    println!("{:?}",account_ctl.get_password_from_account(&"aaa".to_string()).unwrap_or_default());


}