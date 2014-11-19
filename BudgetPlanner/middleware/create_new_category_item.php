<?php

ini_set('display_errors',1); // error reporting
error_reporting(E_ALL);

$response = array();
$scriptResult["response"] = array();

if(isset($_POST['user_id']) && isset($_POST['category_name']) && isset($_POST['category_amount']) && isset($_POST['category_type']) && isset($_POST['category_budget']))
{
	
	$userId = $_POST['user_id'];
	$categoryName = $_POST['category_name'];
    $categoryAmount = $_POST['category_amount'];
    $categoryType = $_POST['category_type'];
    $categoryBudget = $_POST['category_budget'];

	// include db connect class
    require_once __DIR__ . '/db_connect.php';
 
    // connecting to db
    $db = new DB_CONNECT();



    // mysql inserting a new row
    $result = mysql_query("INSERT INTO categoryitems(user_id, category_name, category_amount, category_type, category_budget) 
                            VALUES('$userId', '$categoryName', '$categoryAmount', '$categoryType','$categoryBudget')");
     
    
    // check if row inserted or not
    if ($result) {
        // successfully inserted into database
        $response["success"] = 1;
        $response["message"] = "Successfully added row";
     
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
    
} else {
    // required field is missing
    $response["success"] = 0;
    $response["message"] = "The required fields for this query are missing! Something must have gone wrong!";
 
    // echoing JSON response
    array_push($scriptResult["response"], $response);
    echo json_encode($scriptResult);
}
?>