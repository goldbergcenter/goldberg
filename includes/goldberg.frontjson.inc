<?php
/**
 * @file
 * Generate a json page to query against for the front page
 *
 * @ingroup goldberg
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

  // get the nids, weights, and titles out of the database
  if (variable_get('goldberg_debug', FALSE) === FALSE) {
    // Setup a cache ID
    $cid = 'GOLDBERG:all_panels_query_result';
    // Check for a cached version of the query
    if ($cached = cache_get($cid)) {
      // If a cached entry exists, return it
      $all_panels_query_result = $cached->data;
    }
    else {
      // Run the query
      $all_panels_query_result = db_query($all_panels_query)->fetchAll();
      // Cache the query
      cache_set($cid, $all_panels_query_result, 'cache', strtotime(variable_get('goldberg_cache_duration', '+1 hour')));
    }
  }
  else {
    // Run the query
    $all_panels_query_result = db_query($all_panels_query)->fetchAll();
  }

  // build our json to output

  // Open our json
  print '{' . PHP_EOL;
  print '  "panels": {' . PHP_EOL;
  print '    "panel": [' . PHP_EOL;

  // Initialize our counter
  $counter = 0;
  // Get the number of panels in our query
  $panel_counter = $all_panels_query_result->rowCount();
  
  // Loop over the individual panels
  foreach ($all_panels_query_result as $item) {

    // Output the panel
    print '{' . PHP_EOL;
    print '  "type": "' . $item->type . '"' . PHP_EOL;
    print '}';

    // If we are on the last panel, do not add a comma
    if ($counter == $panel_counter - 1) {
      print PHP_EOL;
    }
    else {
      print ',' . PHP_EOL;
    }
    
    // Increment our counter
    $counter++;

  }

  // Close our json
  print '    ]' . PHP_EOL;
  print '  }' . PHP_EOL;
  print '}';

}