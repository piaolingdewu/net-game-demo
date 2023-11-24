#![allow(warnings)]
mod server;
mod store;
mod user;
mod service;
mod config;
mod controller;

use actix_web::{get, web, App, HttpServer, Responder, HttpResponse, HttpRequest};
use actix_web::cookie::time::Duration;
use actix_web::middleware::{Condition, Logger, NormalizePath};
use log::{info, logger, trace};
use redis::RedisResult;
use tokio::{join, select};
use tokio::signal::unix::{signal, SignalKind};
//use tokio::time::delay_for;




#[get("/")]
async fn index(http:HttpRequest) -> impl Responder {

    println!("date:{} ip:{}",chrono::Utc::now().format("%d/%m/%Y %H:%M"),http.connection_info().peer_addr().unwrap().to_string());
    "Hello, World!"
}

#[get("/{name}")]
async fn hello(name: web::Path<String>) -> impl Responder {
    format!("Hello {}!", &name)
}

// @TODO:只能由本地访问

#[tokio::main]
async fn main(){
    let port=7001;
    


    // 新建默认的专用服务器
    // 验证储存服务器是否存在
    match controller::SERVERCONTROLLER.lock().unwrap().valid_store_server() {
        Ok(client) => {
            println!("储存客户端连接成功!")
        }
        Err(err) => {
            println!("储存客户端连接失败!:{:?}",err)
        }
    }
    match controller::SERVERCONTROLLER.lock(){
        Ok(mut ctl) => {
            // 清除五秒没有心跳包的服务器
            //ctl.cls_dead_ds(1000*5);
            
            // 删除没有记录的服务器
            //ctl.remove_unrecord_server();
            // 删除超时的服务器
            //ctl.remove_timeout_server();
            ctl.remove_all_server();

            // 新增专用服务器
            ctl.extent_ds(3);
            // 新增登录服务器
            ctl.extent_login_ds(1);
            //ctl.extent_ds(3);
            println!("创建默认专用服务器成功!")
        }
        Err(err) => {
            println!("创建默认专用服务器 失败!:{:?}",err)
        }
    }

    let balance=tokio::spawn(async{


        let stop_signal=tokio::spawn(async{
            return tokio::signal::ctrl_c().await;
        });

        let balance_func=tokio::spawn(async{
            loop{
                //return;
                tokio::time::sleep(tokio::time::Duration::from_millis(2500)).await;

                match controller::SERVERCONTROLLER.try_lock() {
                    Ok(ref mut ctl) => {
                        ctl.balance_dedicate_server();
                    },
                    Err(err) => {
                        //println!("error:{}",err);
                    },
            }
            }
        });
        let abort_handel=balance_func.abort_handle();


        select! {
            result = stop_signal =>{
                abort_handel.abort();
                println!("stop!!");
            }
            result = balance_func =>{

            }
        }
        // 检查长时间没有更新的服务器
        // 移除没有更新的服务器
    });

    let server_run=async {
        HttpServer::new(|| App::new()
        .service(service::valid_account_exists)
        .service(service::valid_account)
        .service(service::get_account_info)
        .service(service::register)
        .service(service::join_server)
        .service(service::get_all_servers)
        .service(service::get_free_server)
        .service(service::quit_server)
        .service(service::heartbeat)
        .service(service::get_player_score)
        .service(service::update_player_score)
        .service(service::add_server)
        .service(service::stop_server)
        .service(service::get_all_servers_info)
        .service(service::get_server_info)
        .service(service::try_sign_login)
        .service(service::try_sign_logout)
        .service(service::update_players)
        .service(service::create_room)
        .service(service::get_server_from_name)
        .service(index))
        //.service(hello))
        .bind(("0.0.0.0", port)).unwrap_or_else(|err|{println!("bind port fail:{}",err);panic!()})
        .client_disconnect_timeout(std::time::Duration::from_secs(1))
        .workers(10)
        .run()
        .await.unwrap_or_else(|err|{println!("run server fail:{:?}",err);panic!()});
    };
    //server_run.await;
    join!(server_run,balance);
}

