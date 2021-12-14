package com.WWU.CyberEnvironment.BLE.repository.models;

public class LoginRequestDto {
    String username;
    String password;

    public LoginRequestDto(String username, String password) {
        this.username = username;
        this.password = password;
    }
}
