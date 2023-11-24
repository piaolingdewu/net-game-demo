// Fill out your copyright notice in the Description page of Project Settings.


#include "Type/GameType.h"


FString FServerInfo::to_json() const
{
	return FString::Format(
	TEXT("{\"server_addr\":{\"ip\":\"{0}\",\"port\":\"{1}\"},\"server_heartbeat\":{\"last_hart_beat_time\":{2},\"last_10_ticks_average_time\":{3},\"last_tick_time\":{4},\"status\":\"{5}\"}}"),{
	FString(server_addr.ip),
	FString(server_addr.port),
	FString::FromInt(server_heartbeat.last_hart_beat_time),
	FString::FromInt(server_heartbeat.last_10_ticks_average_time),
	FString::FromInt(server_heartbeat.last_tick_time),
	FString(server_heartbeat.status)});
	
}
