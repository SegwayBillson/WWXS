package com.WWU.CyberEnvironment.BLE.repository;

import com.WWU.CyberEnvironment.BLE.repository.models.LoginRequestDto;
import com.WWU.CyberEnvironment.BLE.repository.models.AuthenticatedUserDto;
import com.WWU.CyberEnvironment.BLE.repository.models.RegistrationRequestDto;
import com.WWU.CyberEnvironment.BLE.repository.models.StationDto;
import java.util.List;
import retrofit2.Call;
import retrofit2.http.Body;
import retrofit2.http.GET;
import retrofit2.http.POST;

public interface WeatherApi {
    @POST("/user/login")
    Call<AuthenticatedUserDto> userLogin(@Body LoginRequestDto dto);

    @POST("/user")
    Call<AuthenticatedUserDto> userRegister(@Body RegistrationRequestDto dto);

    @GET("/buoy")
    Call<List<StationDto>> getStations();
}
