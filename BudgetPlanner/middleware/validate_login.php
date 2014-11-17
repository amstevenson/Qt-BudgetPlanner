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

    // mysql retrieving registration status
    $result = mysql_query("SELECT user_id, user_registered FROM users WHERE user_email = '$userEmail' AND user_password = '$userPassword'");
 
	// check if row retrieved or not
    if(mysql_num_rows($result) > 0)
    {
        while ($row = mysql_fetch_array($result)) 
        {
            // match succeeds
            $response["user_id"] = $row["user_id"];
            $response["user_registered"] = $row["user_registered"];
            $response["success"] = 1;
            $response["message"] = "Query has been successfully processed";
        }
        // echoing JSON response
        array_push($scriptResult["response"], $response);  
        echo json_encode($scriptResult);
        
    } else {
        // failed to retrieve row
        // match fails
        $response["user_registered"] = 0;
        $response["success"] = 0; 
        $response["message"] = "Query returned no match";
 
        // echoing JSON response
        array_push($scriptResult["response"], $response);
        echo json_encode($scriptResult);
    }
} else {
    // required field(s) missing
    $response["success"] = 0;
    $response["message"] = "The required fields for this query are missing! Something must have gone wrong!";
 
    // echoing JSON response
    array_push($scriptResult["response"], $response);
    echo json_encode($scriptResult);
}
?>