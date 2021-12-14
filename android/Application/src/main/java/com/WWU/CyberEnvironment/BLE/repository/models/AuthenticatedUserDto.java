package com.WWU.CyberEnvironment.BLE.repository.models;

public class AuthenticatedUserDto {
    public String token;
    public int id;
//    public int groupId;
    public String error;

    public AuthenticatedUserDto(String token, String error){
        this.token = token;
        this.error = error;
    }
}
