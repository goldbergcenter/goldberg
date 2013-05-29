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

  // get the nids, weights, and titles out of the database
  $all_panels_query = "
    SELECT {goldberg_panels}.nid, {goldberg_panels}.weight, {goldberg_panels}.type, {node}.title
    FROM {goldberg_panels}
    LEFT JOIN {node} ON {node}.nid = {goldberg_panels}.nid
    ORDER BY {goldberg_panels}.weight
  ";
  $all_panels_query_result = db_query($all_panels_query);

  // build our json to output

  print '{' . PHP_EOL;
  print '  "panels": {' . PHP_EOL;
  print '    "panel": [' . PHP_EOL;

  $counter = 0;
  $panel_counter = $all_panels_query_result->rowCount();;
  foreach ($all_panels_query_result as $item) {

    print '{' . PHP_EOL;
    print '  "type": "' . $item->type . '"' . PHP_EOL;
    print '}';

    if ($counter == $panel_counter - 1) {
      print PHP_EOL;
    }
    else {
      print ',' . PHP_EOL;
    }
    $counter++;

  }

  print '    ]' . PHP_EOL;
  print '  }' . PHP_EOL;
  print '}';

}

?>

