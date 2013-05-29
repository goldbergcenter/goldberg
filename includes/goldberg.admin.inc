<?php
/**
 * @file
 * Admin page callbacks for the Pathauto module.
 *
 * @ingroup goldberg
 */

/**
 * Form builder; Select the front page management mode.
 *
 * @ingroup forms
 * @see system_settings_form()
 */
function goldberg_frontpage_select_form($form, &$form_state) {

  // Create a parent element
  $form['select_management'] = array(
    '#tree' => TRUE
  );

  // Create our management mode options
  $type_options = array('manual' => 'Manual', 'auto' => 'Automatic');

  // Display our management mode dropdown
  $form['select_management']['management'] = array(
    '#title' => 'Management Mode',
    '#type' => 'select',
    '#options' => $type_options,
    '#default_value' => variable_get('goldberg_select_management', 'manual'),
  );

  // Create our cache duration options
  $duration_options = array(
    '+1 hour' => 'One hour',
    '+6 hours' => 'Six hours',
    '+12 hours' => 'Twelve hours',
    '+1 day' => 'One day',
    '+3 days' => 'Three days (recommended)',
    '+5 days' => 'Five days',
    '+2 weeks' => 'Two weeks',
    '+3 weeks' => 'Three weeks',
    '+1 month' => 'One month',
    '+3 months' => 'Three months',
    '+6 months' => 'Six months',
    '+9 months' => 'Nine months',
    '+1 year' => 'One year',
    '+5 years' => 'Five years'
  );
  
  // Display our cache duration dropdown
  $form['select_management']['cache_duration'] = array(
    '#title' => 'Cache Duration',
    '#type' => 'select',
    '#options' => $duration_options,
    '#default_value' => variable_get('goldberg_cache_duration', '+1 hour'),
  );
  
  // Create our debug status options
  $debug_options = array(
    true => 'On',
    false => 'Off'
  );
  
  // Display our debug status dropdown
  $form['select_management']['goldberg_debug'] = array(
    '#title' => 'Debug Mode',
    '#type' => 'select',
    '#options' => $debug_options,
    '#default_value' => variable_get('goldberg_debug', false),
  );

  // Define the submit callback
  $form['#submit'][] = 'goldberg_frontpage_select_form_submit';

  // Return the form
  return system_settings_form($form);
}

/**
 *
 * Implements hook_submit().
 *
 * Front page management submission handler.
 */
function goldberg_frontpage_select_form_submit($form, &$form_state) {

  // Set the global management mode to our submitted value
  variable_set('goldberg_select_management', $form_state['values']['select_management']['management']);

  // Set the cache duration to our submitted value
  variable_set('goldberg_cache_duration', $form_state['values']['select_management']['cache_duration']);
  
  // Set the debug status to our submitted value
  variable_set('goldberg_debug', $form_state['values']['select_management']['goldberg_debug']);

}

/**
 * Form builder; Order the front page panels.
 *
 * @ingroup forms
 * @see system_settings_form()
 */
function goldberg_frontpage_form($form, &$form_state) {

  // Confirm that this form is enabled
  if(variable_get('goldberg_select_management', 'manual') == 'manual') {

    // Load all panel data from the database
    include_once('goldberg.paneldata.inc');
    $paneldata = goldberg_panel_fetchdata();

    // Update the panel data and prep it for our form
    $panel_list = goldberg_panel_build_form_data($paneldata);

    // Create a parent element and use our custom theme
    $form['panels'] = array(
      '#prefix' => '<div id="panel-form">',
      '#suffix' => '</div>',
      '#tree' => TRUE,
      '#theme' => 'goldberg_theme_name'
    );

    // Create our image size options
    $type_options = array('large' => 'Large (238x238)', 'small' => 'Small (118x118)', 'tall' => 'Tall (238x118)', 'long' => 'Long (118x238)');

    // Create the form elements for each panel
    foreach ($panel_list as $key => $panel) {

      // Display the panel name with a link to edit the primary node
      $form['panels'][$key]['label'] = array(
        '#type' => 'item',
        '#title' => '<a href="/node/' . $panel['key'] . '/edit">' . $panel['name'] . '</a>',
      );

      // Define the node ID of the panel
      $form['panels'][$key]['key'] = array(
        '#value' => $panel['key'],
        '#type' => 'hidden',
      );

      // Define the size of the panel
      $form['panels'][$key]['type'] = array(
        '#type' => 'select',
        '#options' => $type_options,
        '#default_value' => $panel['type'],
      );

      // Define the order of the panels (needed for table drag)
      $form['panels'][$key]['weight'] = array(
        '#type' => 'textfield',
        '#default_value' => $panel['rank'],
        '#size' => 3,
        '#attributes' => array('class' => array('rank-weight')),
      );
    }

    // Define the submit callback
    $form['#submit'][] = 'goldberg_frontpage_form_submit';

    // Return the form
    return system_settings_form($form);

  } else {

    // The form is disabled, display a notice
    drupal_set_message(t('This form is disabled. Front page management is set to automatic. To enable this form set the management mode to manual.'), 'warning');

  }
}

/**
 *
 * Implements hook_submit().
 *
 * Front page manual management submission handler.
 */
function goldberg_frontpage_form_submit($form, &$form_state) {

  // Loop over our form data
  for($i=0; $i < count($form_state['values']['panels']); $i++) {

    // Get values from our form data for the current record
    $current_key = $form_state['values']['panels'][$i]['key'];
    $current_type = $form_state['values']['panels'][$i]['type'];
    $current_weight = $form_state['values']['panels'][$i]['weight'];

    // Query to update the current record
    $update_query = "
    UPDATE {goldberg_panels}
    SET
    type = :type,
    weight = :weight
    WHERE nid = :key
    ";

    // Update the current record
    $update_query_result = db_query($update_query, array(':key' => $current_key, ':type' => $current_type, ':weight' => $current_weight));
  }
}

/**
 *
 * Custom function.
 *
 * Build the data to be used in the form display.
 */
function goldberg_panel_build_form_data($paneldata) {

  // Get the highest existing weight value
  $last_weight = 0;
  // Define the query
  $get_highest_weight_query = "
    SELECT weight
    FROM {goldberg_panels}
    ORDER BY weight
  ";
  // Run the query
  $get_highest_weight_result = db_query($get_highest_weight_query);
  // Loop over the query results
  foreach ($get_highest_weight_result as $weights) {
    // Always set the variable, so that at the end of the loop the last, and highest, value is set
    $last_weight = $weights->weight;
  }

  // Loop over the node IDs passed in via $paneldata
  for($p = 0; $p < count($paneldata); $p++) {

    // Set up our core variables
    $current_nid = $paneldata[$p]['nid'];
    $current_nid_title = $paneldata[$p]['page_title'];
    $current_weight = '';

    // Check to see if the current item has a weight set
    // Define the weight query
    $weight_query = "
      SELECT weight
      FROM {goldberg_panels}
      WHERE nid = :current_nid
    ";
    // Run the query
    $weight_query_result = db_query($weight_query, array(':current_nid' => $current_nid));
    // Loop over the query
    foreach ($weight_query_result as $item) {
      // Assign the weight returned to the variable
      $current_weight = $item->weight;
    }

    // If a weight was not assigned in the weight query, we need to insert this item into the panel database
    if ($current_weight == '' && isset($current_nid)) {
      // Increment the last weight retreived
      $last_weight++;
      // Define the weight insert query
      $insert_query = "
        INSERT INTO {goldberg_panels} (nid, weight, type)
        VALUES (:nid, :weight, :type)
      ";
      // Run the query
      $insert_query_result = db_query($insert_query, array(':nid' => $current_nid, ':weight' => $last_weight, ':type' => 'small'));
    }
  }

  // Get the nids, weights, types, and titles out of the database
  // Define the query
  $all_weights_query = "
    SELECT {goldberg_panels}.nid, {goldberg_panels}.weight, {goldberg_panels}.type, {node}.title
    FROM {goldberg_panels}
    LEFT JOIN {node} ON {node}.nid = {goldberg_panels}.nid
    ORDER BY {goldberg_panels}.weight
  ";
  $all_weights_query_result = db_query($all_weights_query);

  // Build our working array
  $panel_list = array();
  // Initialize a panel counter
  $panel_counter = 0;
  // Loop over our panels for the listing
  foreach ($all_weights_query_result as $item) {
    // Assign our core values to our array
    $panel_list[$panel_counter]['key'] = $item->nid;
    $panel_list[$panel_counter]['name'] = $item->title;
    $panel_list[$panel_counter]['rank'] = $item->weight;

    // If the type is not assigned, apply a default value, otherwise, assign it
    if ($item->type == '') {
      // Define the default value
      $current_type = 'small';
    }
    else {
      // Assign the type passed from our query
      $current_type = $item->type;
    }
    // Assign the finalized type to our array
    $panel_list[$panel_counter]['type'] = $current_type;
    // Increment our paenl counter
    $panel_counter++;
  }

  // Return the panel array
  return $panel_list;
}

/**
 * Form builder; Automatically order the front page panels.
 *
 * @ingroup forms
 * @see system_settings_form()
 */
function goldberg_frontpage_auto_form($form, &$form_state) {

  // Confirm that this form is enabled
  if(variable_get('goldberg_select_management', 'manual') == 'auto') {

    // ------------------------------------------------------------------------------------------ //

    // The array structure looks like this:
    // $panels[]['nid'];
    // $panels[]['weight'];
    // $panels[]['type'];
    // $panels[]['points'];
    // $panels[]['pointsadjust'];

    // Fetch pattern preset
    // (NOTE: this returns a weight and type array only,
    // we still need points to determine our nids to insert before we can use the array.
    // Pattern presets only impact the layout of the panels and not the content that appears in the panels.)
    //
    // Preset ID 99 returns a randomized panel arrangement
    // Preset ID 1 returns the default panel arrangement
    //
    // This works as a drop down with the preset IDs in the list (currently only default (1) and random (99))
    //--goldberg_fetch_preset();

    // Calculate panel scores
    //
    // All panels have an associated score. These are initially based on visits to the target node (pulled from node_counter table (nid, totalcount)).
    // the points field in the goldberg database is a raw value based on totalcount
    // The points adjust field in the goldberg database is a row that contains promotion/demotion points added both manually and programmatically
    // Via the promote/demote actions points can be added to specific nodes to increase their rankings
    // Panel scores determine the order that all panels appear in
    // The Harvey Goldberg Panel is always the first panel
    // Panels may be manually "promoted to top" where they will appear immediately after the Harvey Goldberg Panel
    //--goldberg_panel_score();

    // Automatically promotes and demotes panels to give lower traffic panels more exposure
    //goldberg_panel_autoadjust();

    // Adds 100 points to the selected panel
    //--goldberg_panel_manual_adjust($panel_id, 'promote');

    // Removes 100 points from a panel (if a negative number, sets it to 0)
    //--goldberg_panel_manual_adjust($panel_id, 'demote');

    // ------------------------------------------------------------------------------------------ //

    // Initialize an array to store our preset in
    $preset = array();
    // Get the preset, we don't need to pass an ID here because we want the default
    $preset = goldberg_fetch_preset();

    // Initilize an array to store the panel pattern in
    $panels = array();
    // Get the panel scores as it currently stands so we can populate our form
    $initial_panels = goldberg_panel_score();

    // Assign types to our panels
    $panel_list = goldberg_assign_types($preset, $initial_panels);

    // Build our form

    // Define the size of the panel
    $form['autoadjust'] = array(
      '#type' => 'button',
      '#default_value' => 'Run Auto-Adjustment',
      '#suffix' => '<br />Warning: Running the auto-adjustment will erase any customizations made to the sort order and points.<hr />',
      '#ajax' => array(
        'callback' => 'goldberg_panel_autoadjust',
      ),
    );

    // Create a parent element and use our custom theme
    $form['panels'] = array(
      '#prefix' => '<div id="panel-form">',
      '#suffix' => '</div>',
      '#tree' => TRUE,
      '#theme' => 'goldberg_theme_name'
    );

    // Create our image size options
    $type_options = array('large' => 'Large (238x238)', 'small' => 'Small (118x118)', 'tall' => 'Tall (238x118)', 'long' => 'Long (118x238)');

    // Create the form elements for each panel
    foreach ($panel_list as $key => $panel) {

      // Display the panel name with a link to edit the primary node
      $form['panels'][$key]['label'] = array(
        '#type' => 'item',
        '#title' => '<a href="/node/' . $panel['nid'] . '/edit">' . $panel['title'] . '</a>',
      );

      // Define the node ID of the panel
      $form['panels'][$key]['key'] = array(
        '#value' => $panel['nid'],
        '#type' => 'hidden',
      );

      // Define the node ID of the panel
      $form['panels'][$key]['panel_key'] = array(
        '#value' => $panel['id'],
        '#type' => 'hidden',
      );

      // Define the size of the panel
      $form['panels'][$key]['type'] = array(
        '#type' => 'select',
        '#options' => $type_options,
        '#default_value' => $panel['type'],
      );

      if ($panel['title'] != 'Harvey Goldberg') {

        // Define the size of the panel
        $form['panels'][$key]['promote'] = array(
          '#type' => 'button',
          '#default_value' => '+',
          '#id' => 'promote' . $panel['id'],
          '#ajax' => array(
            'callback' => 'goldberg_frontpage_auto_promote_form',
          ),
        );

        // Define the size of the panel
        $form['panels'][$key]['demote'] = array(
          '#type' => 'button',
          '#default_value' => '-',
          '#id' => 'demote' . $panel['panel_id'],
          '#ajax' => array(
            'callback' => 'goldberg_frontpage_auto_demote_form',
          ),
        );

        $total_points = $panel['pointsadjust'] + $panel['points'];

        // Define the size of the panel
        $form['panels'][$key]['points'] = array(
          '#type' => 'item',
          '#title' => $total_points,
        );

      }

      // Define the order of the panels (needed for table drag)
      $form['panels'][$key]['weight'] = array(
        '#type' => 'textfield',
        '#default_value' => $panel['weight'],
        '#size' => 3,
        '#attributes' => array('class' => array('rank-weight')),
      );
    }

    // Define the form submission callback
    $form['#submit'][] = 'goldberg_frontpage_auto_form_submit';

    // Return the form
    return system_settings_form($form);

  } else {
    // The form is disabled, display a notice
    drupal_set_message(t('This form is disabled. Front page management is set to manual. To enable this form set the management mode to automatic.'), 'warning');
  }
}

function goldberg_frontpage_auto_promote_form($form, &$form_state) {

print_r($form_state['values']['panels'][5]['key']);
break;


  // Get the current row we are on
  $row_id = $form_state['triggering_element']['#parents'][1];
  // Using the row the the nid of the panel item
  $panel_key = $form_state['values']['panels'][$row_id]['panel_key'];
  // Adjust the panel score
  goldberg_panel_manual_adjust($panel_key, 'promote');
  $form_state['rebuild'] = TRUE;
  drupal_set_message(t('Value updated.'), 'warning');
  return render($form);
}
function goldberg_frontpage_auto_demote_form($form, &$form_state) {

  // Get the current row we are on
  $row_id = $form_state['triggering_element']['#parents'][1];
  // Using the row the the nid of the panel item
  $panel_key = $form_state['values']['panels'][$row_id]['panel_key'];
  // Adjust the panel score
  goldberg_panel_manual_adjust($panel_key, 'demote');
  $form_state['rebuild'] = TRUE;
  return render($form);
}


/**
 *
 * Implements hook_submit().
 *
 * Front page automatic management submission handler.
 */
function goldberg_frontpage_auto_form_submit($form, &$form_state) {

  // Loop over our form data
  for($i=0; $i < count($form_state['values']['panels']); $i++) {

    // Get values from our form data for the current record
    $current_key = $form_state['values']['panels'][$i]['key'];
    $current_type = $form_state['values']['panels'][$i]['type'];
    $current_weight = $form_state['values']['panels'][$i]['weight'];

    // Query to update the current record
    $update_query = "
    UPDATE {goldberg_panels}
    SET
    type = :type,
    weight = :weight
    WHERE nid = :key
    ";

    // Update the current record
    $update_query_result = db_query($update_query, array(':key' => $current_key, ':type' => $current_type, ':weight' => $current_weight));
  }

}

/**
 * Store presets for panel arrangements.
 *
 * Note: Presets are only 18 panels. The system will automatically add any additional panels
 * to the end of the list and set their type to small by default.
 */
function goldberg_fetch_preset($preset_id = 1) {

  // Initialize the return array
  $preset = array();
  // Initialize a temporary array that will hold our preset values
  $temp_preset = array();

  // Choose a preset
  switch($preset_id) {
    case 1:
      // The default pattern
      $temp_preset[] = 'large';
      $temp_preset[] = 'small';
      $temp_preset[] = 'small';
      $temp_preset[] = 'tall';
      $temp_preset[] = 'long';
      $temp_preset[] = 'tall';
      $temp_preset[] = 'tall';
      $temp_preset[] = 'small';
      $temp_preset[] = 'small';
      $temp_preset[] = 'tall';
      $temp_preset[] = 'small';
      $temp_preset[] = 'small';
      $temp_preset[] = 'large';
      $temp_preset[] = 'tall';
      $temp_preset[] = 'small';
      $temp_preset[] = 'long';
      $temp_preset[] = 'small';
      $temp_preset[] = 'long';
      break;
    case 99:
      // If the preset requested was random, return a randomly arranged panel pattern

      // Loop over a default of 18 panels
      for ($i=0; $i < 18; $i++) {
        // Create a random number between 1 and 4
        $rand_result = rand(1, 4);
        // Add the corresponding type to our array
        if ($rand_result == 1) {
          $temp_preset[] = 'large';
        }
        if ($rand_result == 2) {
          $temp_preset[] = 'small';
        }
        if ($rand_result == 3) {
          $temp_preset[] = 'long';
        }
        if ($rand_result == 4) {
          $temp_preset[] = 'tall';
        }
      }
      break;
    default:
      // The default pattern
      $temp_preset[] = 'large';
      $temp_preset[] = 'small';
      $temp_preset[] = 'small';
      $temp_preset[] = 'tall';
      $temp_preset[] = 'long';
      $temp_preset[] = 'tall';
      $temp_preset[] = 'tall';
      $temp_preset[] = 'small';
      $temp_preset[] = 'small';
      $temp_preset[] = 'tall';
      $temp_preset[] = 'small';
      $temp_preset[] = 'small';
      $temp_preset[] = 'large';
      $temp_preset[] = 'tall';
      $temp_preset[] = 'small';
      $temp_preset[] = 'long';
      $temp_preset[] = 'small';
      $temp_preset[] = 'long';
      break;
  }

  // Loop over our temporary preset and add it to our return array
  for ($i=0; $i < count($temp_preset); $i++) {
    // Add the weight to each element in the array
    $preset[$i]['weight'] = $i + 1;
    // Add the type from our preset to the array
    $preset[$i]['type'] = $temp_preset[$i];
  }

  // Return our array
  return $preset;
}

function goldberg_panel_score() {
  // Calculate panel score
  //
  // All panels have an associated score. These are initially based on visits to the target node (pulled from node_counter table (nid, totalcount)).
  // the points field in the goldberg database is a raw value based on totalcount
  // The points adjust field in the goldberg database is a row that contains promotion/demotion points added both manually and programmatically
  // Via the promote/demote actions points can be added to specific nodes to increase their rankings
  // Panel scores determine the order that all panels appear in
  // The Harvey Goldberg Panel is always the first panel
  // Panels may be manually "promoted to top" where they will appear immediately after the Harvey Goldberg Panel

  // Get the totalcount and write it to the points field in the goldberg_panels database
  // Get points adjust and add it to the newly updated raw points
  // Get the highest value (accounting for both points and adjusted points)
  // Promote Harvey to top
  // Check for "promoted to top" nodes and Promote to just below Harvey

  // Get the current panel counts
  $all_totalcount_query = "
    SELECT node_counter.totalcount, goldberg_panels.id, goldberg_panels.points, goldberg_panels.pointsadjust
    FROM node_counter
    LEFT JOIN field_data_field_content ON field_data_field_content.field_content_nid = node_counter.nid
    LEFT JOIN goldberg_panels ON goldberg_panels.nid = field_data_field_content.entity_id
  ";
  // Run the query
  $all_totalcount_query_result = db_query($all_totalcount_query);
  // Loop over the query result
  foreach ($all_totalcount_query_result as $item) {
    $update_points_query = "
      UPDATE goldberg_panels
      SET points = :totalcount
      WHERE id = :id
    ";
    // Run the query
    $update_points_query_result = db_query($update_points_query, array(':totalcount' => $item->totalcount, ':id' => $item->id));
  }

  // Define the query
  $get_panels_query = "
    SELECT node.nid, node.title, goldberg_panels.id, goldberg_panels.nid AS panel_id, goldberg_panels.points, goldberg_panels.pointsadjust, goldberg_panels.weight, goldberg_panels.promote
    FROM node
    LEFT JOIN field_data_field_content ON field_data_field_content.field_content_nid = node.nid
    LEFT JOIN goldberg_panels ON goldberg_panels.nid = field_data_field_content.entity_id
    WHERE node.type != 'panel'
  ";
  // Run the query
  $get_panels_query_result = db_query($get_panels_query);
  $harvey_key = 'NULL';
  $panels = array();
  $counter = 0;
  foreach ($get_panels_query_result as $item) {
    $panels[$counter]['nid'] = $item->nid;
    $panels[$counter]['title'] = $item->title;
    $panels[$counter]['id'] = $item->id;
    $panels[$counter]['panel_id'] = $item->panel_id;
    $panels[$counter]['points'] = $item->points;
    $panels[$counter]['pointsadjust'] = $item->pointsadjust;
    $panels[$counter]['totalpoints'] = $item->points + $item->pointsadjust;
    $panels[$counter]['weight'] = $item->weight;
    $counter++;
  }

  // Sort the panels so we can add our promoted items
  // Obtain a list of columns
  foreach ($panels as $key => $row) {
    $panelsort[$key] = $row['totalpoints'];
  }

  // Sort the data with totalpoints descending
  // Add $panels as the last parameter, to sort by the common key
  array_multisort($panelsort, SORT_DESC, $panels);

  // Store the highest count value
  $highest_count = $panels[0]['totalpoints'] + 1;

  $counter = 0;
  // Loop over the newly sorted array
  for ($i=0; $i < count($panels); $i++) {
    if ($panels[$counter]['title'] == "Harvey Goldberg") {
      $panels[$counter]['promote'] = 2;
      $harvey_key = $counter;
    }
    else {
      $panels[$counter]['promote'] = $item->promote;
    }
    if ($panels[$counter]['promote'] == 1) {
      $panels[$current_key]['totalpoints'] = $highest_count;
      $highest_count++;
    }
    $counter++;
  }

  // Make sure harvey is promoted to the top
  $highest_count++;
  $panels[$harvey_key]['totalpoints'] = $highest_count;

  // Sort the panels by totalpoints again
  // Obtain a list of columns
  foreach ($panels as $key => $row) {
    $pointssort[$key] = $row['totalpoints'];
  }

  // Sort the data with totalpoints descending
  // Add $panels as the last parameter, to sort by the common key
  array_multisort($pointssort, SORT_DESC, $panels);

  // Export the array of panels
  return $panels;
}

function goldberg_panel_autoadjust() {
  // Automatically promotes and demotes panels to give lower traffic panels more exposure

  // Get the current panel counts
  $current_panel_count_query = "
    SELECT node_counter.totalcount, goldberg_panels.id, goldberg_panels.points, goldberg_panels.pointsadjust
    FROM node_counter
    LEFT JOIN field_data_field_content ON field_data_field_content.field_content_nid = node_counter.nid
    LEFT JOIN goldberg_panels ON goldberg_panels.nid = field_data_field_content.entity_id
    ORDER BY node_counter.totalcount
  ";
  // Run the query
  $current_panel_count_query_result = db_query($current_panel_count_query);
  // Initialize a counter for the array
  $counter = 0;
  // Loop over the query result
  foreach ($current_panel_count_query_result as $panel_count) {
    // Store the total combined points
    $current_count[$counter]['count'] = $panel_count->totalcount + $panel_count->pointsadjust;
    // Store the raw points (actual counter value)
    $current_count[$counter]['points'] = $panel_count->totalcount;
    // Store the id of the panel
    $current_count[$counter]['id'] = $panel_count->id;
    $counter++;
  }

  // Obtain a list of columns
  foreach ($current_count as $key => $row) {
      $panelcount[$key]  = $row['count'];
  }

  // Sort the data with count descending
  // Add $current_count as the last parameter, to sort by the common key
  array_multisort($panelcount, SORT_DESC, $current_count);

  $total_panels = count($current_count);
  if ($total_panels < 8) {
    // There are not enough panels to sort, exit the function
    return;
  }

  // Store the highest count value
  $highest_count = $current_count[0]['count'];

  // Divide panels into 8 groups
  $section_size = $total_panels / 8;

  // Group 1 demote to bottom
  $section_start = 0;
  $section_end = $section_size;
  for ($i = $section_start; $i < $section_end; $i++) {
    $current_count[$i]['count'] = 0;
  }

  // Group 2 randomize
  $section_end = $section_size * 2;
  $section_start = $section_end - $section_size;
  for($i = $section_start; $i < $section_end; $i++) {
    $current_item = $current_count[$i]['count'];
    $current_item = rand(0, $highest_count);
    if ($current_item < 0) {
      $current_item = 0;
    }
    $current_count[$i]['count'] = $current_item;
  }

  // Group 7 randomize
  $section_end = $section_size * 7;
  $section_start = $section_end - $section_size;
  for($i = $section_start; $i < $section_end; $i++) {
    $current_item = $current_count[$i]['count'];
    $current_item = rand(0, $highest_count);
    if ($current_item < 0) {
      $current_item = 0;
    }
    $current_count[$i]['count'] = $current_item;
  }

  // Group 8 promote to top
  $section_end = $section_size * 8;
  $section_start = $section_end - $section_size;
  for($i = $section_start; $i < $section_end; $i++) {
    $current_item = $current_count[$i]['count'];
    $current_item = $highest_count + 100;
    $current_count[$i]['count'] = $current_item;
  }

  // Groups 3-6 keep the same

  // Update the goldberg database with our new counts
  for($i = 0; $i < count($current_count); $i++) {
    $final_adjust_points = $current_count[$i]['count'] - $current_count[$i]['points'];
    if ($final_adjust_points < 0) {
      $final_adjust_points = 0;
    }
    $update_query = "
      UPDATE goldberg_panels
      SET pointsadjust = :adjustment
      WHERE id = :id
    ";

    // Run the query
    $update_query_result = db_query($update_query, array(':adjustment' => $final_adjust_points, ':id' => $current_count[$i]['id']));
  }

}

function goldberg_panel_manual_adjust($panel_id, $direction) {
  // Adds 100 points to the selected panel

  // Get the current adjustment score
  $promote_prepare_query = "
  SELECT pointsadjust
  FROM goldberg_panels
  WHERE nid = :panel_id
  ";
  // Run the query
  $promote_prepare_query_result = db_query($promote_prepare_query, array(':panel_id' => $panel_id));

  // Loop over the query result
  foreach ($promote_prepare_query_result as $panel_scores) {
    $current_score = $panel_scores->pointsadjust;
  }

  if ($direction == 'promote') {
    // Add 100 points to the retreived score
    $current_score = $current_score + 100;
  }
  else if ($direction == 'demote') {
    // Subtract 100 points from the retreived score
    $current_score = $current_score - 100;
    // Set the score to 0 if the adjustment makes this a negative value
    if ($current_score < 0) {
      $current_score = 0;
    }
  }

  // Update the score with the new value
  $promote_query = "
  UPDATE goldberg_panels
  SET pointsadjust = :adjustment
  WHERE nid = :panel_id
  ";

  // Run the query
  $promote_query_result = db_query($promote_query, array(':adjustment' => $current_score, ':panel_id' => $panel_id));

}

/**
 * Assign types passed from the preset to the sorted array of panels
 */
function goldberg_assign_types($preset, $panels) {

  $preset_size = count($preset);
  $panels_size = count($panels);
  $extra_loops = 0;
  $loop_size = $panels_size;

  if ($preset_size < $panels_size) {
    // If the panel size is larger we need to fill in the extra slots that the preset doesn't cover
    $loop_size = ($preset_size - $panels_size) + $panels_size;
  }

  // Loop over the presets
  for ($i = 0; $i < $loop_size; $i++) {
    // Assign the corresponding preset types to the panels
    $panels[$i]['type'] = $preset[$i]['type'];
    if ($i > $panels_size) {
      $panels[$i]['type'] = 'small';
    }
  }

  // Return the new array
  return $panels;
}