import { Component, OnInit, Input } from '@angular/core';
import { ApiService } from '../services/api.service';
import { ActivatedRoute } from '@angular/router';
import { IDropdownSettings } from 'ng-multiselect-dropdown';

@Component({
  selector: 'line-graph',
  templateUrl: './line-graph.component.html',
  styleUrls: ['./line-graph.component.css']
})
export class LineGraphComponent implements OnInit{

  //fields for selected buoys drop down
  dropdownList;
  selected_items = [];
  dropdownSettings = {
    singleSelection: false,
    idField: 'item_id',
    textField: 'item_text',
    selectAllText: 'Select All',
    unSelectAllText: 'UnSelect All',
    itemsShowLimit: 2,
    allowSearchFilter: true
  };

  //array that will hold the two different arrays that our graph with display
  tempdata;
  //an integer used to switch between the two different arrays above
  flip = 0;

  /* chart options for drop down, add charts to give user more options */
  charts = ['Line Chart','Scatter Chart'];
  chartMap = new Map([
    ['Line Chart', 'LineChart'],
    ['Scatter Chart', 'ScatterChart']
  ]);
  type = this.chartMap.get('Line Chart'); // default chart shown


  /* data type options for drop down, should probably rename these i.e. 'surf temperature' rather than 'surfTemp' */
  dataTypes = ['Surface Temperature', 'Shallow Salinity', 'Surface Insolation', 'Depth Turbidity', 'Shallow Temperature', 'Depth Temperature']; 
  dataMap = new Map([
    ['Surface Temperature', 'surfTemp'],
    ['Shallow Salinity', 'shallowSalinity'],
    ['Surface Insolation', 'surfInsolation'],
    ['Depth Turbidity', 'depthTurbidity'],
    ['Shallow Temperature', 'shallowTemp'],
    ['Depth Temperature', 'depthTemp']
  ]);
  dataType = this.dataMap.get('Surface Temperature'); // default graph shown
  
  //var to hold data passed in ngOnInit(), maybe shouldn't hold all of it?
  buoy_data: any[];
  title: string; // title of the chart
  data;
  column_names: any[];
  width: number;
  height: number;
  monthNames = ["January", "February", "March", "April", "May", "June",
  "July", "August", "September", "October", "November", "December"];

  /* options for Google Chart */
  options = {   
    hAxis: {
       title: 'Month'
    },
    vAxis:{
       title: 'Surface Temperature'
    },
  };

  queryParam: number;

  /* constructor */
  constructor(private api: ApiService, private route: ActivatedRoute){}

  ngOnInit(): void {

    this.route.queryParams.subscribe(params => {
      this.queryParam = params['id'];
    });

    /* get data from database */
    this.api.getData().subscribe(data => {

      this.buoy_data = Object.assign(Object.create(data), data); // get deep copy of data

      //populate buoy drop down
      var len = data.length;
      var count = 1;
      //var templist = [];
      this.dropdownList = new Array();
      for(var i = 0; i < len; i++){
        //we only need one instance of each name... are buoy names going to be unique at release?
        if(this.buoyNameExists(data[i].buoy.name, this.dropdownList))
          this.dropdownList.push({item_id: count++, item_text: data[i].buoy.name});
      }

      this.width = 550; // for Google Chart purposes
      this.height = 400;
      this.adjustColumns();
      this.plotData();
    });
  }

  /* function to plot the data */
  plotData(): void {
    //create two 2d arrays to be held by tempdata for this.data to point to
    this.tempdata = Array.from({length: 2}, () => Array.from({length: 12}, () => Array.from({length: this.column_names.length})));
    //populate tempdata with months
    var i = 0;
    this.monthNames.forEach(element => {
      this.tempdata[0][i][0] = element;
      this.tempdata[1][i][0] = element;
      i++;
    });
    var timestamp_date;
    var data_month;
    var buoy_column;

    //looping through buoyData array
    for (let buoy_index = 0; buoy_index < this.buoy_data.length; buoy_index++) {
      timestamp_date = new Date(this.buoy_data[buoy_index].timestamp);
      data_month = timestamp_date.getMonth();
      buoy_column = this.getColumnIndex(buoy_index);

      //the user didn't select the buoy to be visible
      if(buoy_column == -1)
        continue;

      //probably a more efficient way to do this outside of the for loop
      switch(this.dataType){
        /* Surface Temperature */
        case 'surfTemp': 
          console.log("Fell in surfTemp");
          console.log("this.buoyData["+buoy_index+"] = " + this.buoy_data[buoy_index].surfTemp);
          console.log("Did I fall here??");
          this.title = "Surface Temperature Data"; // update title of graph   
          this.options.vAxis.title = "Surface Temp in Fahrenheit"; // update y-axis label
          this.tempdata[this.flip][data_month][buoy_column] = this.buoy_data[buoy_index].surfTemp;
          break;

        /* Shallow Temperature */
        case 'shallowTemp': 
          console.log("Fell in shallowTemp");
          console.log("this.buoyData["+buoy_index+"] = " + this.buoy_data[buoy_index].shallowTemp);
          console.log("Did I fall here??");
          this.title = "Surface Temperature Data"; // update title of graph   
          this.options.vAxis.title = "Surface Temp in Fahrenheit"; // update y-axis label
          this.tempdata[this.flip][data_month][buoy_column] = this.buoy_data[buoy_index].shallowTemp;
          break;

        /* Depth Temperature */
        case 'depthTemp': 
          console.log("Fell in depthTemp");
          console.log("this.buoyData["+buoy_index+"] = " + this.buoy_data[buoy_index].depthTemp);
          console.log("Did I fall here??");
          this.title = "Surface Temperature Data"; // update title of graph   
          this.options.vAxis.title = "Surface Temp in Fahrenheit"; // update y-axis label
          this.tempdata[this.flip][data_month][buoy_column] = this.buoy_data[buoy_index].depthTemp;
          break;

        /* Surface Salinity */
        case 'surfSalinity':
          console.log("Fell in surfSalinity");
          console.log("this.buoyData["+buoy_index+"] = " + this.buoy_data[buoy_index].surfSalinity);
          console.log("Did I fall here??");
          this.title = "Surface Salinity Data"; // update title of graph
          this.options.vAxis.title = "Surface Salinity in what units?"; // update y-axis label
          this.tempdata[this.flip][data_month][buoy_column] = this.buoy_data[buoy_index].surfSalinity;
          break;

        /* Shallow Salinity */
        case 'shallowSalinity': 
          console.log("Fell in shallowSalinity");
          console.log("this.buoyData["+buoy_index+"] = " + this.buoy_data[buoy_index].shallowSalinity);
          console.log("Did I fall here??");
          this.title = "Shallow Salinity Data"; // update title of graph
          this.options.vAxis.title = "Shallow Salinity in what units?"; // update y-axis label
          this.tempdata[this.flip][data_month][buoy_column] = this.buoy_data[buoy_index].shallowSalinity;
          break;

        /* Surface Insolation */
        case 'surfInsolation': 
          console.log("Fell in surfInsolation");
          console.log("this.buoyData["+buoy_index+"] = " + this.buoy_data[buoy_index].surfInsolation);
          console.log("Did I fall here??");
          this.title = "Surface Insolation Data"; // update title of graph  
          this.options.vAxis.title = "Surface Insolation in what units?"; // update y-axis label
          this.tempdata[this.flip][data_month][buoy_column] = this.buoy_data[buoy_index].surfInsolation;
          break;

        /* Depth Turbidity */
        case 'depthTurbidity':
          console.log("Fell in depthTurbidity");
          console.log("this.buoyData["+buoy_index+"] = " + this.buoy_data[buoy_index].depthTurbidity);
          console.log("Did I fall here??");
          this.title = "Depth Turbidity Data"; // update title of graph   
          this.options.vAxis.title = "Depth Turbidity in what units?"; // update y-axis label
          this.tempdata[this.flip][data_month][buoy_column] = this.buoy_data[buoy_index].depthTurbidity;
          break;
      }
    }

    //for some reason the graph won't update unless we assign a completely different array
    //so we keep flipping between the two arrays in tempdata and assign data to the newest one
    this.data = this.tempdata[this.flip];

    /* flip which array we'll be editing on the next call to plotdata() */
    if(this.flip)
      this.flip = 0;
    else
      this.flip = 1;
  }

  //checks if a buoy name exists within the buoy drop down
  buoyNameExists(name: string, dropdown: {item_id: number, item_text: string}[]): boolean {
    var len = dropdown.length;
    for(var i = 0; i < len; i++){
      if(name === dropdown[i].item_text)
        return false;
    }
    return true;
  }

  //get index of a buoy in column_names given an index of a datapoint in buoy_data
  getColumnIndex(index: number): number{

    var len = this.column_names.length;
    //search each index of our column names to find the one that matches our buoy name
    // we start at 1 because index 0 is 'months'
    for(var i = 1; i < len; i++){
      if(this.column_names[i] === this.buoy_data[index].buoy.name)
        return i;
    }

    //didn't exist
    return -1;
  }

  //adjusts columns of our google chart
  //these columns make up the selected buoys
  adjustColumns(): void{
    this.column_names = ["Month"];
    this.selected_items.forEach(element => {
      //add a column for each buoy name
      this.column_names.push(element.item_text);
    });
  }

  /* drop down chart type event handler */
  onChartSelect(event){
    this.type = this.chartMap.get(event.target.value);
  }

  /* Function to handle drop-down data-type event handler */
  onDataTypeSelect(event){
    this.dataType = this.dataMap.get(event.target.value);
    console.log("dataType from onDataTypeSelect = " + this.dataType);
    this.dataType = event.target.value;
    this.plotData();
  }

  //event handler for single select of buoy drop down
  onBuoySelect(item: any) {
    this.adjustColumns();
    this.plotData();
  }
  //event handler for single deselect of buoy drop down
  onBuoyDeSelect(item: any) {
    this.adjustColumns();
    this.plotData();
  }
  //event handler for select all of buoy drop down
  onBuoySelectAll(items: any) {
    this.adjustColumns();
    this.plotData();
  }
  //event handler for DeSelect all of buoy drop down
  onBuoyDeSelectAll(items: any) {
    this.adjustColumns();
    this.plotData();
  }
}