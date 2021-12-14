import { Component, OnInit } from '@angular/core';
import { ActivatedRoute } from '@angular/router';
import { ApiService } from '../services/api.service';

@Component({
  selector: 'app-data',
  templateUrl: './data.component.html',
  styleUrls: ['./data.component.css']
})
export class DataComponent implements OnInit {

  constructor(private api: ApiService, private route: ActivatedRoute) { }

  data: any[];

  queryParam: number;

  ngOnInit(): void {
    this.route.queryParams.subscribe(params => {
      this.queryParam = params['id'];
    });
    this.api.getData().subscribe(data => {
      this.data = data;
      console.log("filling unsorted data from data.component.ts")
      console.log(data);
    })
  }
}
