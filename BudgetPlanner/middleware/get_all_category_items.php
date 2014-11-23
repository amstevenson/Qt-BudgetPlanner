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

    // mysql getting all rows
    $result = mysql_query("SELECT * FROM categoryitems WHERE user_id = '$userId'");
     
    // check for empty result
    if (mysql_num_rows($result) > 0) {

        $response["categoryitems"] = array();
     
        while ($row = mysql_fetch_array($result)) {
            // temp user array
            $categoryItem = array();
            $categoryItem["category_name"] = $row["category_name"];
            $categoryItem["category_amount"] = $row["category_amount"];
            $categoryItem["category_type"] = $row["category_type"];
            $categoryItem["category_budget"] = $row["category_budget"];

            // push single category item into final response array
            array_push($response["categoryitems"], $categoryItem);
        }
        // success
        $response["success"] = 1;
     
        // echoing JSON response
        echo json_encode($response);
    }
    else
    {
        // technical error retrieving rows
        $response["success"] = 0;
        $response["message"] = "The database could not find any information, please try again or contact addstevenson@hotmail.com ";
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