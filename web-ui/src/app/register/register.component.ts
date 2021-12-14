import { Component, OnInit } from '@angular/core';
import { FormControl, FormGroup } from '@angular/forms';
import { Router } from '@angular/router';
import { ApiService } from '../services/api.service';
import { AuthService } from '../services/auth.service';

@Component({
  selector: 'app-register',
  templateUrl: './register.component.html',
  styleUrls: ['./register.component.css']
})
export class RegisterComponent implements OnInit {

  public errorMessage: string;

  constructor(private router: Router, private auth: AuthService, private api: ApiService) { }

  ngOnInit(): void {
  }

  registerForm = new FormGroup({
    username: new FormControl(''),
    email: new FormControl(''),
    phonenumber: new FormControl(''),
    password: new FormControl(''),
    password2: new FormControl('')
  });

  async onSubmit() {
    // Validate form
    if (this.registerForm.value.password !== this.registerForm.value.password2) {
      this.errorMessage = 'Passwords do not match';
    }
    else {
      try {
        const user = await this.api.postUser(this.registerForm.value).toPromise(); /* encrypt password */
        this.auth.loginUser(user)
      }
      catch (e) {
        console.log(e);
        this.errorMessage = `Wrong login: error ${e.status}: ${e.statusText}`;
      }
    }
    // If token is valid and user is logged in, send 'em home!
    if (this.auth.isAuth()) {
      this.router.navigateByUrl('/home');
    }
  }

}
