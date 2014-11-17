<?php

ini_set('display_errors',1); // error reporting
error_reporting(E_ALL);

$response = array();
$scriptResult["response"] = array();

if(isset($_POST['user_email']) && isset($_POST['user_password']))
{
	
	$userEmail = $_POST['user_email'];
	$userPassword = $_POST['user_password'];

	// include db connect class
    require_once __DIR__ . '/db_connect.php';
 
    // connecting to db
    $db = new DB_CONNECT();


    $check = mysql_query("SELECT user_email FROM users WHERE user_email = '$userEmail'");

    if(mysql_num_rows($check) == 0)
    {
        // mysql inserting a new row
        $result = mysql_query("INSERT INTO users(user_email, user_password) VALUES('$userEmail', '$userPassword')");
     
    	// check if row inserted or not
        if ($result) {
            // successfully inserted into database
            $response["success"] = 1;
            $response["message"] = "You have successfully created your account, please login using these credentials.";
     
            // echoing JSON response
            array_push($scriptResult["response"], $response);  
            echo json_encode($scriptResult);
        } else {
            // failed to insert row
            $response["success"] = 0;
            $response["message"] = "There has been a technical error; please contact addstevenson@hotmail.com.";
     
            // echoing JSON response
            array_push($scriptResult["response"], $response);
            echo json_encode($scriptResult);
        }
    }
    else 
    {
        // email address already taken
        $response["success"] = 0;
        $response["message"] = "This email address is already in use. Please contact addstevenson@hotmail.com for more advice.";
     
        // echoing JSON response
        array_push($scriptResult["response"], $response);
        echo json_encode($scriptResult);
    }
} else {
    // required field is missing
    $response["success"] = 0;
    $response["message"] = "The required fields for this query are missing! Something must have gone wrong!";
 
    // echoing JSON response
    array_push($scriptResult["response"], $response);
    echo json_encode($scriptResult);
}
?>