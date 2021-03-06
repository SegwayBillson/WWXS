package com.WWU.CyberEnvironment.BLE.repository;

import android.os.AsyncTask;
import android.util.Log;

import com.WWU.CyberEnvironment.BLE.repository.models.GetStationsResponseDto;
import com.WWU.CyberEnvironment.BLE.repository.models.LoginRequestDto;
import com.WWU.CyberEnvironment.BLE.repository.models.AuthenticatedUserDto;
import com.WWU.CyberEnvironment.BLE.repository.models.RegistrationRequestDto;
import com.WWU.CyberEnvironment.BLE.repository.models.StationDto;
import com.squareup.moshi.Moshi;

import java.io.IOException;
import java.util.List;
import okhttp3.OkHttpClient;
import retrofit2.Response;
import retrofit2.converter.moshi.MoshiConverterFactory;
import retrofit2.Retrofit;

@SuppressWarnings("StaticFieldLeak")
public class Repository implements DataSource{
    @Override
    public AuthenticatedUserDto userLogin(final String username, final String password) {
        if (isNullOrEmpty(username) || isNullOrEmpty(password)) {
            return null;
        }

        LoginRequestDto requestDto = new LoginRequestDto(username, password);
        try {
            return new doUserLogin().execute(requestDto).get();
        } catch (Exception e) {
            e.printStackTrace();
            return null;
        }
    }

    @Override
    public AuthenticatedUserDto userRegister(String username, String email, String password) {
        if (isNullOrEmpty(email) || isNullOrEmpty(password)) {
            return null;
        }

        RegistrationRequestDto requestDto = new RegistrationRequestDto(username, email, password);
        try {
            return new doUserRegister().execute(requestDto).get();
        } catch (Exception e) {
            e.printStackTrace();
            return null;
        }
    }

    @Override
    public GetStationsResponseDto getStations() {
        try {
            return new doGetStations().execute().get();
        } catch (Exception e) {
            e.printStackTrace();
            return null;
        }
    }

    private class doUserLogin extends AsyncTask<LoginRequestDto, Void, AuthenticatedUserDto> {
        @Override
        protected AuthenticatedUserDto doInBackground(LoginRequestDto... loginRequestDtos) {
            try {
                Response<AuthenticatedUserDto> response = weatherApi.userLogin(loginRequestDtos[0]).execute();

                if (response.isSuccessful()) {
                    return response.body();
                } else {
                    return new AuthenticatedUserDto(null, ErrorParser.parse(response.errorBody().string()));
                }
            } catch (IOException e) {
                e.printStackTrace();
                return null;
            } catch (NullPointerException e) {
                e.printStackTrace();
                return null;
            }
        }
    }
    private class doUserRegister extends AsyncTask<RegistrationRequestDto, Void, AuthenticatedUserDto> {
        @Override
        protected AuthenticatedUserDto doInBackground(RegistrationRequestDto... registrationRequestDtos) {
            try {
                Response<AuthenticatedUserDto> response = weatherApi.userRegister(registrationRequestDtos[0]).execute();

                if (response.isSuccessful()) {
                    return response.body();
                } else {
                    return new AuthenticatedUserDto(null, ErrorParser.parse(response.errorBody().string()));
                }
            } catch (IOException e) {
                e.printStackTrace();
                return null;
            } catch (NullPointerException e) {
                e.printStackTrace();
                return null;
            }
        }
    }

    private class doGetStations extends AsyncTask<Void, Void, GetStationsResponseDto> {
        @Override
        protected GetStationsResponseDto doInBackground(Void... voids) {
            try {
                Response<List<StationDto>> response = weatherApi.getStations().execute();

                if (response.isSuccessful()) {
                    Log.d("GETBUOY", "doInBackground: response is successful");
                    GetStationsResponseDto stationResponse = new GetStationsResponseDto(null);
                    stationResponse.stations = response.body();
                    return stationResponse;
                } else {
                    return new GetStationsResponseDto(ErrorParser.parse(response.errorBody().string()));
                }
            } catch (IOException e) {
                e.printStackTrace();
                return null;
            } catch (NullPointerException e) {
                e.printStackTrace();
                return null;
            }
        }
    }

    private boolean isNullOrEmpty(String s) {
        return s == null || s.isEmpty();
    }

    private Moshi moshi = new Moshi.Builder()
            .build();

    private OkHttpClient client = new OkHttpClient.Builder()
            .addInterceptor(new RequestInterceptor())
            .build();

    private WeatherApi weatherApi = new Retrofit.Builder()
            .baseUrl("http://10.0.2.2:8080/")
            .addConverterFactory(MoshiConverterFactory.create(moshi))
            .client(client)
            .build()
            .create(WeatherApi.class);
}


