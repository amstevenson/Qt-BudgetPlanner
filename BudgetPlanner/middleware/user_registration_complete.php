<?php

ini_set('display_errors',1); // error reporting
error_reporting(E_ALL);

$response = array();
$scriptResult["response"] = array();

if(isset($_POST['user_id']))
{
	
	$userId = $_POST['user_id'];

	// include db connect class
    require_once __DIR__ . '/db_connect.php';
 
    // connecting to db
    $db = new DB_CONNECT();

    // mysql inserting a new row
    $result = mysql_query("UPDATE users SET user_registered = '1' WHERE user_id = '$userId'");
     
    // check if changed or not
    if ($result) {
        // successfully changed the row in the database
        $response["success"] = 1;
        $response["message"] = "Successfully changed the row - user has been registered";
     
        // echoing JSON response
        array_push($scriptResult["response"], $response);  
        echo json_encode($scriptResult);
    } else {
        // failed to change row
        $response["success"] = 0;
        $response["message"] = "There has been a technical error; please contact addstevenson@hotmail.com.";
     
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