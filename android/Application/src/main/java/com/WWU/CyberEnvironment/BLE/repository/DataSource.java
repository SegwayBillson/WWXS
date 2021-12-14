package com.WWU.CyberEnvironment.BLE.repository;

import com.WWU.CyberEnvironment.BLE.repository.models.AuthenticatedUserDto;
import com.WWU.CyberEnvironment.BLE.repository.models.GetStationsResponseDto;

public interface DataSource {
    AuthenticatedUserDto userLogin(String username, String password);
    AuthenticatedUserDto userRegister(String username, String email, String password);
    GetStationsResponseDto getStations();
}
