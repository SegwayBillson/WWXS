package com.WWU.CyberEnvironment.BLE.repository.models;

public class RegistrationRequestDto {
    String email;
    String username;
    String password;

    public RegistrationRequestDto(String username, String email, String password) {
        this.username = username;
        this.email = email;
        this.password = password;
    }
}