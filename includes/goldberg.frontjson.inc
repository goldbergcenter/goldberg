<?php
/**
 * @file
 * Generate a json page to query against for the front page
 *
 * @ingroup goldberg
 */

/**
 * Fetch and display json to be passed via AJAX to the front page
 */
function goldberg_frontjson() {

  // Set our header
  header('Content-type: application/json');

  // Disable devel output on this page
  $GLOBALS['devel_shutdown'] = FALSE;

  // Construct the query
  $all_panels_query = "
    SELECT {goldberg_panels}.nid, {goldberg_panels}.weight, {goldberg_panels}.type, {node}.title
    FROM {goldberg_panels}
    LEFT JOIN {node} ON {node}.nid = {goldberg_panels}.nid
    ORDER BY {goldberg_panels}.weight
  ";

  // build our json to output
   
  // Check to see if we are in debug mode or not
  if (variable_get('goldberg_debug', FALSE) === FALSE) {
    // Setup cache IDs
    $query_cid = 'GOLDBERG:all_panels_query_result';
    $output_cid = 'GOLDBERG:goldberg_front_json_output';
    // Check for a cached version of the json
    if ($output_cached = cache_get($output_cid)) {
      // If a cached json output entry exists, return it
      // NOTE: If a cached version of the json exists, we don't need to load the cached query
      $json_output = $output_cached->data;
    }
    else {
      // Run the query
      $all_panels_query_result = db_query($all_panels_query)->fetchAll();
      // Build the json
      $json_output = goldberg_build_json($all_panels_query_result);
      // Cache the query
      cache_set($query_cid, $all_panels_query_result, 'cache', strtotime(variable_get('goldberg_cache_duration', '+1 hour')));
      // Cache the json
      cache_set($output_cid, $json_output, 'cache', strtotime(variable_get('goldberg_cache_duration', '+1 hour')));
    }    
  }
  else {
    // Run the query
    $all_panels_query_result = db_query($all_panels_query)->fetchAll();
    // Build the json
    $json_output = goldberg_build_json($all_panels_query_result);
  }

  // Return the json
  print $json_output;

}

/**
 * Construct the json
 */
function goldberg_build_json($all_panels_query_result) {

  // Define our return value
  $json_output = '';

  // Open our json
  $json_output .= '{' . PHP_EOL;
  $json_output .= '  "panels": {' . PHP_EOL;
  $json_output .= '    "panel": [' . PHP_EOL;
  
  // Initialize our counter
  $counter = 0;
  // Get the number of panels in our query
  $panel_counter = count($all_panels_query_result);
  
  // Loop over the individual panels
  foreach ($all_panels_query_result as $item) {
  
    // Output the panel
    $json_output .= '{' . PHP_EOL;
    $json_output .= '  "type": "' . $item->type . '"' . PHP_EOL;
    $json_output .= '}';
  
    // If we are on the last panel, do not add a comma
    if ($counter == $panel_counter - 1) {
      $json_output .= PHP_EOL;
    }
    else {
      $json_output .= ',' . PHP_EOL;
    }
    
    // Increment our counter
    $counter++;
  
  }
  
  // Close our json
  $json_output .= '    ]' . PHP_EOL;
  $json_output .= '  }' . PHP_EOL;
  $json_output .= '}';
  
  // Return the json
  return $json_output;
}