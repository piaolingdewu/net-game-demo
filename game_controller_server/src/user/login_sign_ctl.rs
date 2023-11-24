

// 登录凭证处理

use crate::service::valid_account;
use crate::store::store_ctl::data_ctl;
use crate::store::store_data::Login_Data;

use serde::{Serialize,Deserialize};

pub struct LoginSignCtl {
    login_sign:data_ctl,
}
#[derive(Serialize, Deserialize, Debug)]
struct SignData {
    pub account:String,
    pub password:String,
    pub login_time:i64,
}

pub enum SignLoginFailResult {
    // 登录失败 无效签名
    FailSignInvalid,
    // 登录失败 签名已经存在
    FailSignExist,
    // 登录失败 签名超时
    FailSignTimeout,
}

impl LoginSignCtl {
    // 登录凭证

    pub fn new()->Self{
        let space_name="loginSign";
        let login_sign=data_ctl::new(space_name);
        Self{
            login_sign,
        }
    }


    // 尝试登录
    pub fn try_sign_login(&mut self, account:&Login_Data, time:&i64, sign:&String) -> Result<(), SignLoginFailResult> {

        if self.valid_sign(&account,&time,&sign) {

            return if let Some(data) = self.login_sign.get_data(format!("{}", account.account).as_str()) {
                Err(SignLoginFailResult::FailSignExist)
            } else {
                // 并没有重复登录 可以登录
                let sign_data = SignData {
                    account: account.account.clone().to_string(),
                    password: account.password.clone().to_string(),
                    login_time: time.clone(),
                };
                self.login_sign.update_data(format!("{}", account.account).as_str(), serde_json::to_string(&sign_data).unwrap().as_str());
                Ok(())
            }
        }
        // 签名无效 不给登录
        return Err(SignLoginFailResult::FailSignInvalid);
    }

    // 尝试登出
    pub fn try_sign_logout(&mut self, account:&Login_Data, time:&i64, sign:&String) -> bool {

        if self.valid_sign(&account,&time,&sign) {
            return if let Some(data) = self.login_sign.get_data(format!("{}", account.account).as_str()) {
                self.login_sign.delete_data(format!("{}", account.account).as_str());
                true
            } else {
                false
            }
        }
        return false;
    }


    // 验证一个登录凭证 是否有效 登录凭证的计算方法是 md5("{Account},{Password},{time}")
    pub fn valid_sign(&mut self,account:&Login_Data, time:&i64 ,sign:&String) -> bool {
        // @todo: 如果凭证距离现在太久 那么就不允许登录

        let primary_sign=md5::compute(format!("{},{},{}",account.account,account.password,time));


        return if format!("{:x}",primary_sign)==sign.clone() {
            true
        } else {
            false
        }
    }


}

#[test]
fn try_login(){

    let mut LoginSignCtl=LoginSignCtl::new();
    let account=Login_Data{
        account: "test".to_string(),
        password: "test".to_string(),
    };
    let time=chrono::Utc::now().timestamp_micros();
    let sign=format!("{:x}",md5::compute(format!("{},{},{}",account.account,account.password,time)));
    match LoginSignCtl.try_sign_login(&account.clone(),&time,&sign.clone()) {
        Ok(_) => {
            println!("登录成功")
        }
        Err(err) => {

            match err {
                SignLoginFailResult::FailSignInvalid => {
                    println!("登录失败 签名无效")
                }
                SignLoginFailResult::FailSignExist => {
                    println!("登录失败 已经存在重复签名")
                }
                SignLoginFailResult::FailSignTimeout => {
                    println!("登录失败 签名超时")

                }
            }
        }
    }

    // 尝试登出
    if LoginSignCtl.try_sign_logout(&account.clone(),&time,&sign.clone()) {
        // 登出成功
        println!("登出成功")

    }else {
        println!("登出失败")
    }

}