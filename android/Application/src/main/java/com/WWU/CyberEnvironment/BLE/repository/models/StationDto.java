package com.WWU.CyberEnvironment.BLE.repository.models;

public class StationDto {
    public int id;              // Buoy DB ID
    public String name;         // Buoy nickname
    public String mac;          // Buoy Bluetooth MAC address
    public String pubKey;       // PubKey to encrypt info for buoy
    public String lastRetrieval;// Last date that buoy submitted data
    public String version;      // Buoy version
    public String createdAt;    // Date buoy is added to DB
    public String updatedAt;    // Date buoy info was last updated

    // FIXME: null value from DB cannot be parsed as int, will error-out.
//    public int groupId;         // Owner Ids

//    public String url;
    public String latitude;     // Buoy latitude location
    public String longitude;    // Buoy longitude location
}
