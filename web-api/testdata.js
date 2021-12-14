const testData = { // TODO: Associate records
  users: [
    {
      username: 'kris',
      password: 'admin',
      role: 'dev',
      groupId: 1
    },
    {
      username: 'erik',
      password: 'admin',
      role: 'dev',
      groupId: 1
    },
    {
      username: 'john',
      password: 'password',
      role: 'user',
      groupId: 2
    }
  ],
  groups: [
    {
      name: 'default'
    },
    {
      name: 'group1'
    }
  ],
  buoys: [
    {
      name: 'proto',
      mac: 'FF-FF-FF-FF-FF-FF',
      groupId: 1
    },
    {
      name: 'salish',
      mac: '12:34:56:78:9A:BC',
      groupId: 2
    }
  ],
  data: [
    {
      timestamp: new Date(2021, 5, 1),
      surfTemp: Math.random() * 100,
      surfInsolation: Math.random(),
      shallowSalinity: Math.random(),
      shallowTemp: Math.random(),
      depthTemp: Math.random(),
      depthTurbidity: Math.random(),
      buoyId: 1,
      userId: 1
    },
    {
      timestamp: new Date(2021, 6, 1),
      surfTemp: Math.random() * 100,
      surfInsolation: Math.random(),
      shallowSalinity: Math.random(),
      shallowTemp: Math.random(),
      depthTemp: Math.random(),
      depthTurbidity: Math.random(),
      buoyId: 1,
      userId: 1
    },
    {
      timestamp: new Date(2021, 7, 1),
      surfTemp: Math.random() * 100,
      surfInsolation: Math.random(),
      shallowSalinity: Math.random(),
      shallowTemp: Math.random(),
      depthTemp: Math.random(),
      depthTurbidity: Math.random(),
      buoyId: 1,
      userId: 1
    },
    {
      timestamp: new Date(2021, 8 , 1),
      surfTemp: Math.random() * 100,
      surfInsolation: Math.random(),
      shallowSalinity: Math.random(),
      shallowTemp: Math.random(),
      depthTemp: Math.random(),
      depthTurbidity: Math.random(),
      buoyId: 2,
      userId: 1
    },
    {
      timestamp: new Date(2021, 9, 1),
      surfTemp: Math.random() * 100,
      surfInsolation: Math.random(),
      shallowSalinity: Math.random(),
      shallowTemp: Math.random(),
      depthTemp: Math.random(),
      depthTurbidity: Math.random(),
      buoyId: 2,
      userId: 1
    },
    {
      timestamp: Date(2021, 10, 1),
      surfTemp: Math.random() * 100,
      surfInsolation: Math.random(),
      shallowSalinity: Math.random(),
      shallowTemp: Math.random(),
      depthTemp: Math.random(),
      depthTurbidity: Math.random(),
      buoyId: 2,
      userId: 1
    }
  ]
};

module.exports = testData;
