import { Injectable } from '@angular/core';
import { Router } from '@angular/router';
import { ApiService } from './api.service';

@Injectable({
  providedIn: 'root'
})
export class AuthService {

  /**
   * return true if the user is logged in, false if they are not
   * @returns boolean if the user is logged in
   */
  public isAuth(): boolean {
    return this.getLocalToken() && !this.isExpired(this.getLocalToken());
  }

  /**
   * retreives the stored local token 
   * @returns stored local token 
   */
  public getLocalToken(): string {
    return this.getUserData().token;
  }

  /**
   * retreives the stored user data
   * @returns stored user data
   */
  public getUserData(): any {
    if (localStorage.getItem('user')) {
      return JSON.parse(localStorage.getItem('user'));
    }
    else {
      return '';
    }
  }

  /**
   * logs in a user from an api response
   * @param user api user JSON
   */
  public loginUser(user: any): boolean {
    if (this.isExpired(user.token)) {
      return false;
    }
    else {
      localStorage.setItem('user', JSON.stringify(user));
      return true;
    }
  }

  /**
   * Logs out current user.
   */
  logout() {
    localStorage.clear();
    this.router.navigateByUrl('');
  }

  /**
   * checks if a jwt is expired 
   * @param token string jwt token
   * @returns expired
   */
  public isExpired(token: string): boolean {
    return this.getTokenTimestamp(token) < (Date.now() / 1000);
  }

  /**
   * gets a timestamp from a token 
   * @param token string jwt token
   * @returns time stamp in epoch format
   */
  public getTokenTimestamp(token: string): number {
    return JSON.parse(atob(token.split('.')[1])).exp;
  }

  constructor(private api: ApiService, private router: Router) {
    /* check if there is a stored user token locally */
    
  }
}
