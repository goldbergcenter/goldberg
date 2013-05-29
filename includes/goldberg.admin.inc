<?php
/**
 * @file
 * Admin page callbacks for the Pathauto module.
 *
 * @ingroup goldberg
 */

/**
 * Form builder; Administer the Goldberg Module.
 *
 * @ingroup forms
 * @see system_settings_form()
 */
function goldberg_frontpage_select_form($form, &$form_state) {

  // Create a parent element
  $form['goldberg_admin'] = array(
    '#tree' => TRUE
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
  $form['goldberg_admin']['cache_duration'] = array(
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
  $form['goldberg_admin']['goldberg_debug'] = array(
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

  // Set the cache duration to our submitted value
  variable_set('goldberg_cache_duration', $form_state['values']['goldberg_admin']['cache_duration']);
  
  // Set the debug status to our submitted value
  variable_set('goldberg_debug', $form_state['values']['goldberg_admin']['goldberg_debug']);

}

/**
 * Form builder; Order the front page panels.
 *
 * @ingroup forms
 * @see system_settings_form()
 */
function goldberg_frontpage_form($form, &$form_state) {
  
  // Load all panel data from the database
  include_once('goldberg.paneldata.inc');
  $paneldata = goldberg_panel_fetchdata();
  
  // Update the panel data and prep it for our form
  $panel_list = goldberg_panel_build_form_data($paneldata);
  
  // Create a parent element and use our custom theme
  $form['panel_form']['panels'] = array(
    '#prefix' => '<div id="panel-form">',
    '#suffix' => '</div>',
    '#tree' => TRUE,
    '#theme' => 'goldberg_theme_name'
  );
  
  // Create our debug status options
  $goldberg_panel_size_options = array(
    0 => 'Manual',
    1 => 'Default',
    99 => 'Random'
  );
  
  // Display our debug status dropdown
  $form['panel_form']['panel_size'] = array(
    '#title' => 'Panel Sizes',
    '#description' => 'Selecting anything but Manual will override the panel sizes, but not their order.',
    '#type' => 'select',
    '#options' => $goldberg_panel_size_options,
    '#weight' => 0,
    '#default_value' => variable_get('goldberg_panel_size_preset', 0),
  );
  
  // Create our image size options
  $type_options = array('large' => 'Large (238x238)', 'small' => 'Small (118x118)', 'tall' => 'Tall (238x118)', 'long' => 'Long (118x238)');
  
  // Create the form elements for each panel
  foreach ($panel_list as $key => $panel) {
  
    // Display the panel name with a link to edit the primary node
    $form['panel_form']['panels'][$key]['label'] = array(
      '#type' => 'item',
      '#title' => '<a href="/node/' . $panel['key'] . '/edit">' . $panel['name'] . '</a>',
    );
  
    // Define the node ID of the panel
    $form['panel_form']['panels'][$key]['key'] = array(
      '#value' => $panel['key'],
      '#type' => 'hidden',
    );
  
    // Define the size of the panel
    $form['panel_form']['panels'][$key]['type'] = array(
      '#type' => 'select',
      '#options' => $type_options,
      '#default_value' => $panel['type'],
    );
  
    // Define the order of the panels (needed for table drag)
    $form['panel_form']['panels'][$key]['weight'] = array(
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
}

/**
 *
 * Implements hook_submit().
 *
 * Front page manual management submission handler.
 */
function goldberg_frontpage_form_submit($form, &$form_state) {

  // Set the debug status to our submitted value
  variable_set('goldberg_panel_size_preset', $form_state['values']['panel_size']);

  // Store the number of panels
  $num_panels = count($form_state['values']['panels']);

  // Initialize an array to hold our panel preset
  $panels = array();

  // Build our query to update the current record, this will be reused in the loops below
  $update_query = "
  UPDATE {goldberg_panels}
  SET
  type = :type,
  weight = :weight
  WHERE nid = :key
  ";
  
  if (variable_get('goldberg_panel_size_preset', 0) != 0) {
    // Get our array of panel sizes
    $panels = goldberg_fetch_preset(variable_get('goldberg_panel_size_preset', 0), $num_panels);
    
    // Make sure that our return value is the correct size
    if ($num_panels != count($panels)) {
      // Something went horribly wrong, go back to manual
      variable_set('goldberg_panel_size_preset', 0);
      drupal_set_message(t('There was a problem using that preset. Please try again or use the manual preset.'), 'warning');
    } else {
 
      // Loop over our array of panels
      for($i=0; $i < $num_panels; $i++) {
        // Get values from our form data for the current record
        $current_key = $form_state['values']['panels'][$i]['key'];
        $current_weight = $form_state['values']['panels'][$i]['weight'];
        
        // Get the type from our panels array and not the form
        $current_type = $panels[$i]['type'];
    
        // Update the current record
        $update_query_result = db_query($update_query, array(':key' => $current_key, ':type' => $current_type, ':weight' => $current_weight));
      }
      drupal_set_message(t('Preset changes successfully submitted.'), 'status');
    
    }
  }
  
  // Are we using manual ordering
  if (variable_get('goldberg_panel_size_preset', 0) == 0) {

    // Loop over our form data
    for($i=0; $i < $num_panels; $i++) {
  
      // Get values from our form data for the current record
      $current_key = $form_state['values']['panels'][$i]['key'];
      $current_type = $form_state['values']['panels'][$i]['type'];
      $current_weight = $form_state['values']['panels'][$i]['weight'];
  
      // Update the current record
      $update_query_result = db_query($update_query, array(':key' => $current_key, ':type' => $current_type, ':weight' => $current_weight));
    }
    drupal_set_message(t('Manual changes successfully submitted.'), 'status');
    
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
 * Store presets for panel arrangements.
 *
 * Note: Presets are only 18 panels. The system will automatically add any additional panels
 * to the end of the list and set their type to small by default.
 */
function goldberg_fetch_preset($preset_id = 1, $num_panels) {

  // Initialize the array for our preset
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
    // Add the type from our preset to the array
    $preset[$i]['type'] = $temp_preset[$i];
  }
  
  // Initialize our return array
  $panels = array();
  
  // Get the size of the preset
  $preset_size = count($preset);
  // Assign our panel size
  $panels_size = $num_panels;
  // Initialize a loop to contain extra panels that we need to add on
  $extra_loops = 0;
  // Set the initial value of the loop size to the number of panels
  $loop_size = $num_panels;

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

  // Return our array
  return $panels;
}