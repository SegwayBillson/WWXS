import { Component, OnInit } from '@angular/core';
import { FormControl, FormGroup } from '@angular/forms';
import { Router } from '@angular/router';
import { ApiService } from '../services/api.service';
import { AuthService } from '../services/auth.service';

@Component({
  selector: 'app-login',
  templateUrl: './login.component.html',
  styleUrls: ['./login.component.css']
})
export class LoginComponent implements OnInit {

  errorMessage: string;

  constructor(private router: Router, private auth: AuthService, private api: ApiService) { }

  loginForm = new FormGroup({
    username: new FormControl(''),
    password: new FormControl('')
  });

  ngOnInit(): void {
  }

  /* TODO: Encrypt the password */
  async onSubmit() {
    const credentials = this.loginForm.value;
    console.log(credentials);
    // Get token Asynchonusly so it can be stored and validaded before routed to new page.
    try {
      const user = await this.api.postUserLogin(credentials).toPromise(); /* update credentials to encrypted password */
      this.auth.loginUser(user);
    } catch (e) {
      console.log(e);
      this.errorMessage = `Wrong login: error ${e.status}: ${e.statusText}`;
    }
    // If token is valid and user is logged in, send 'em home!
    if (this.auth.isAuth()) {
      this.router.navigateByUrl('/home');
    }
  }

}
