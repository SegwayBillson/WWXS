package com.WWU.CyberEnvironment.BLE.authentication;

import android.animation.Animator;
import android.animation.AnimatorListenerAdapter;
import android.annotation.TargetApi;
import android.app.DialogFragment;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.support.annotation.NonNull;
import android.app.Activity;
import android.app.LoaderManager.LoaderCallbacks;
import android.content.CursorLoader;
import android.content.Loader;
import android.database.Cursor;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.provider.ContactsContract;
import android.text.TextUtils;
import android.view.KeyEvent;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.inputmethod.EditorInfo;
import android.widget.ArrayAdapter;
import android.widget.AutoCompleteTextView;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;
import com.WWU.CyberEnvironment.BLE.DeviceScanActivity;
import com.WWU.CyberEnvironment.BLE.R;
import com.WWU.CyberEnvironment.BLE.repository.Repository;
import com.WWU.CyberEnvironment.BLE.repository.models.AuthenticatedUserDto;
import java.lang.reflect.Field;
import java.util.ArrayList;
import java.util.List;
import static android.Manifest.permission.READ_CONTACTS;

/**
 * A login screen that offers login via email/password.
 */
public class LoginActivity extends Activity implements LoaderCallbacks<Cursor> {
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_login);

        prepareLoginActivity();
    }

    /* Prepare Login Activity
     * Holds the flow control for the user attempting to login.
     */
    private void prepareLoginActivity() {
        mUsernameView = findViewById(R.id.sign_in_username);
        populateAutoComplete();
        mPasswordView = findViewById(R.id.sign_in_password);
        mPasswordView.setOnEditorActionListener(new TextView.OnEditorActionListener() {
            @Override
            public boolean onEditorAction(TextView textView, int id, KeyEvent keyEvent) {
                if (id == EditorInfo.IME_ACTION_DONE || id == EditorInfo.IME_NULL) {
                    attemptLogin();
                    return true;
                }
                return false;
            }
        });

        Button mUsernameSignInButton = findViewById(R.id.username_sign_in_button);
        mUsernameSignInButton.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View view) {
                attemptLogin();
            }
        });

        mLoginFormView = findViewById(R.id.scroll_login_form);
        mProgressView = findViewById(R.id.login_progress);

        TextView registrationButton = findViewById(R.id.registrationText);

        registrationButton.setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View v) {
                DialogFragment registrationFragment = new UserRegistrationFragment();
                registrationFragment.show(getFragmentManager(), "user_registration_fragment");
            }
        });
    }

    /* Populate Auto Complete
     * Checks for ability to autocomplete a text block.
     */
    private void populateAutoComplete() {
        if (!mayRequestContacts()) {
            return;
        }

        getLoaderManager().initLoader(0, null, this);
    }

    /* May Request Contacts
     * Requests the user for permission to use contacts.
     */
    private boolean mayRequestContacts() {
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.M) {
            return true;
        }
        if (checkSelfPermission(READ_CONTACTS) == PackageManager.PERMISSION_GRANTED) {
            return true;
        }
        if (shouldShowRequestPermissionRationale(READ_CONTACTS)) {
            Toast.makeText(getApplicationContext(), R.string.contacts,
                    Toast.LENGTH_LONG).show();
        } else {
            requestPermissions(new String[]{READ_CONTACTS}, REQUEST_READ_CONTACTS);
        }
        return false;
    }

    /* On Request Permissions Result
     * If the user gives the access to contacts then populate the autocomplete.
     */
    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions,
                                           @NonNull int[] grantResults) {
        if (requestCode == REQUEST_READ_CONTACTS) {
            if (grantResults.length == 1 && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                populateAutoComplete();
            }
        }
    }

    /* Attempt Login
     * Handles the flow control for a user clicking the button to attempt to login.
     */
    private void attemptLogin() {
        mUsernameView.setError(null);
        mPasswordView.setError(null);

        final String username = mUsernameView.getText().toString();
        final String password = mPasswordView.getText().toString();

        boolean cancel = false;
        View focusView = null;

        if (!TextUtils.isEmpty(password) && !isPasswordValid(password)) {
            mPasswordView.setError(getString(R.string.error_invalid_password));
            focusView = mPasswordView;
            cancel = true;
        }

        if (TextUtils.isEmpty(username)) {
            mUsernameView.setError(getString(R.string.error_field_required));
            focusView = mUsernameView;
            cancel = true;
        }

        if (cancel) {
            focusView.requestFocus();
        } else {
            new Thread(new Runnable() {
                public void run() {
                    showProgress(true);
                    AuthenticatedUserDto responseDto = repository.userLogin(username, password);
                    handleResponse(responseDto);
                    showProgress(false);
                }
            }).start();
        }
    }

    /* Handle Response
     * Handles the response from the server about the login attempt.
     */
    private void handleResponse(AuthenticatedUserDto response) {
        if(response == null) {
            loginError(getString(R.string.error_login));
            return;
        } else if(response.error != null) {
            loginError(response.error);
            return;
        }

        CurrentUser.key = response.token;
        CurrentUser.id = response.id;
//        CurrentUser.groupId = response.groupId;

        final Intent intent = new Intent(this, DeviceScanActivity.class);

        startActivity(intent);
    }

    /* Login Error
     * Handle the error message of a bad login attempt.
     */
    private void loginError(final String message) {
        runOnUiThread(new Runnable() {
            public void run() {
                mUsernameView.setError(message);
                mUsernameView.requestFocus();
            }
        });
    }

    private boolean isEmailValid(String email) {
        return email.contains("@");
    }
    private boolean isPasswordValid(String password) {
        return password.length() > 2;
    }

    /* Show Progress
     * Displays a spinning wheel for progress.
     */
    @TargetApi(Build.VERSION_CODES.HONEYCOMB_MR2)
    private void showProgress(final boolean show) {
        runOnUiThread(new Runnable() {
            public void run() {
                int shortAnimTime = getResources().getInteger(android.R.integer.config_shortAnimTime);

                mLoginFormView.setVisibility(show ? View.GONE : View.VISIBLE);
                mLoginFormView.animate().setDuration(shortAnimTime).alpha(
                        show ? 0 : 1).setListener(new AnimatorListenerAdapter() {
                    @Override
                    public void onAnimationEnd(Animator animation) {
                        mLoginFormView.setVisibility(show ? View.GONE : View.VISIBLE);
                    }
                });

                mProgressView.setVisibility(show ? View.VISIBLE : View.GONE);
                mProgressView.animate().setDuration(shortAnimTime).alpha(
                        show ? 1 : 0).setListener(new AnimatorListenerAdapter() {
                    @Override
                    public void onAnimationEnd(Animator animation) {
                        mProgressView.setVisibility(show ? View.VISIBLE : View.GONE);
                    }
                });
            }
        });
    }

    /* On Create Loader
     * Loads the users information.
     */
    @Override
    public Loader<Cursor> onCreateLoader(int i, Bundle bundle) {
        return new CursorLoader(this,
                // Retrieve incomingDataBuffer rows for the device user's 'profile' contact.
                Uri.withAppendedPath(ContactsContract.Profile.CONTENT_URI,
                        ContactsContract.Contacts.Data.CONTENT_DIRECTORY), ProfileQuery.PROJECTION,

                // Select only email addresses.
                ContactsContract.Contacts.Data.MIMETYPE +
                        " = ?", new String[]{ContactsContract.CommonDataKinds.Email
                .CONTENT_ITEM_TYPE},

                // Show primary email addresses first. Note that there won't be
                // a primary email address if the user hasn't specified one.
                ContactsContract.Contacts.Data.IS_PRIMARY + " DESC");
    }

    /* On Load Finished
     * If successful load then store the autocomplete info and goto the nest page.
     */
    @Override
    public void onLoadFinished(Loader<Cursor> cursorLoader, Cursor cursor) {
        List<String> emails = new ArrayList<>();
        cursor.moveToFirst();
        while (!cursor.isAfterLast()) {
            emails.add(cursor.getString(ProfileQuery.ADDRESS));
            cursor.moveToNext();
        }

        addEmailsToAutoComplete(emails);
    }

    @Override
    public void onLoaderReset(Loader<Cursor> cursorLoader) {

    }

    /* On Destroy
     * Burn it all.
     */
    @Override
    protected void onDestroy() {
        super.onDestroy();
        try {
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M && Build.MANUFACTURER.equals("samsung")) {
                Object systemService = getSystemService(Class.forName("com.samsung.android.content.clipboard.SemClipboardManager"));
                Field mContext = systemService.getClass().getDeclaredField("mContext");
                mContext.setAccessible(true); mContext.set(systemService, null);
            }
        } catch (Exception e) {}
    }

    private void addEmailsToAutoComplete(List<String> emailAddressCollection) {
        ArrayAdapter<String> adapter =
                new ArrayAdapter<>(LoginActivity.this,
                        android.R.layout.simple_dropdown_item_1line, emailAddressCollection);

        mUsernameView.setAdapter(adapter);
    }

    private interface ProfileQuery {
        String[] PROJECTION = {
                ContactsContract.CommonDataKinds.Email.ADDRESS,
                ContactsContract.CommonDataKinds.Email.IS_PRIMARY,
        };
        int ADDRESS = 0;
        int IS_PRIMARY = 1;
    }

    private final static String TAG = LoginActivity.class.getSimpleName();
    private static final int REQUEST_READ_CONTACTS = 0;

    private AutoCompleteTextView mUsernameView;
    private EditText mPasswordView;
    private View mProgressView;
    private View mLoginFormView;
    private Repository repository = new Repository();
}
