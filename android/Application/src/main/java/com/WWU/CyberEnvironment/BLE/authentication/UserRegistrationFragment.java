package com.WWU.CyberEnvironment.BLE.authentication;

import android.app.DialogFragment;
import android.content.Intent;
import android.graphics.drawable.ColorDrawable;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.text.TextUtils;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.EditText;
import com.WWU.CyberEnvironment.BLE.DeviceScanActivity;
import com.WWU.CyberEnvironment.BLE.R;
import com.WWU.CyberEnvironment.BLE.repository.Repository;
import com.WWU.CyberEnvironment.BLE.repository.models.AuthenticatedUserDto;

public class UserRegistrationFragment extends DialogFragment {
    @Nullable
    @Override
    public View onCreateView(LayoutInflater inflater, @Nullable ViewGroup container, Bundle savedInstanceState) {
        return inflater.inflate(R.layout.dialog_account_registration, container);
    }

    /* On View Created
     * Create the variables to store the user input and prepare dialog.
     */
    @Override
    public void onViewCreated(View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);
        setCancelable(true);

        userName = view.findViewById(R.id.input_username);
        userEmail = view.findViewById(R.id.input_email);
        userPassword = view.findViewById(R.id.input_password);
        userPassword2 = view.findViewById(R.id.input_confirm_password);
        registerButton = view.findViewById(R.id.button_register_account);

        prepareDialog();
    }

    /* On Resume
     * Control flow for when the app is reopened.
     */
    @Override
    public void onResume() {
        super.onResume();

        Window window = getDialog().getWindow();

        if (window == null) {
            dismiss();
            return;
        }

        window.setLayout(
                WindowManager.LayoutParams.MATCH_PARENT,
                WindowManager.LayoutParams.WRAP_CONTENT
        );

        window.setBackgroundDrawable(new ColorDrawable(android.graphics.Color.TRANSPARENT));
    }

    /* Prepare Dialog
     * Scan the user input for input errors and display response.
     */
    private void prepareDialog() {
        registerButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                final String tmpUsername = userName.getText().toString();
                final String tmpEmail = userEmail.getText().toString();
                final String pass1 = userPassword.getText().toString();
                final String pass2 = userPassword2.getText().toString();

                boolean cancel = false;
                View focusView = null;

                if (!TextUtils.isEmpty(pass1) && !isPasswordValid(pass1)) {
                    userPassword.setError(getString(R.string.error_invalid_password));
                    focusView = userPassword;
                    cancel = true;
                }

                if (!pass1.equals(pass2)) {
                    userPassword.setError(getString(R.string.Passwords_dont_match));
                    focusView = userPassword;
                    cancel = true;
                }

                if (TextUtils.isEmpty(tmpUsername)) {
                    userName.setError(getString(R.string.error_field_required));
                    focusView = userName;
                    cancel = true;
                }

                if (TextUtils.isEmpty(tmpEmail)) {
                    userEmail.setError(getString(R.string.error_field_required));
                    focusView = userEmail;
                    cancel = true;
                } else if (!isEmailValid(tmpEmail)) {
                    userEmail.setError(getString(R.string.error_invalid_email));
                    focusView = userEmail;
                    cancel = true;
                }

                if (cancel) {
                    focusView.requestFocus();
                } else {
                    new Thread(new Runnable() {
                        public void run() {
                            AuthenticatedUserDto response = repository.userRegister(tmpUsername, tmpEmail, pass1);
                            validateResponse(response);
                        }
                    }).start();
                }
            }
        });
    }

    /* Validate Response
     * Check response from server and store data.
     */
    private void validateResponse(AuthenticatedUserDto response) {
        if(response == null) {
            registrationError(getString(R.string.error_registration));
            return;
        } else if (response.error != null) {
            registrationError(response.error);
            return;
        }

        CurrentUser.key     = response.token;
        CurrentUser.id      = response.id;
//        CurrentUser.groupId = response.groupId;

        final Intent intent = new Intent(getActivity(), DeviceScanActivity.class);

        startActivity(intent);
    }

    /* Registration Error
     * Sets user registration error and error focus.
     */
    private void registrationError(final String message) {
        getActivity().runOnUiThread(new Runnable() {
            public void run() {
                userEmail.setError(message);
                userEmail.requestFocus();
            }
        });
    }

    private boolean isEmailValid(String email) {
        return email.contains("@");
    }
    private boolean isPasswordValid(String password) {
        return password.length() > 7;
    }

    private EditText userName;
    private EditText userEmail;
    private EditText userPassword;
    private EditText userPassword2;
    private Button registerButton;

    private Repository repository = new Repository();
}


