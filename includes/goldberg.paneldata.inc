<?php

/**
 * @file
 * Function call to grab data for all panels
 */

/**
 * Get content for the panel from the database
 */
function goldberg_panel_fetchdata() {

  // Query to get all panels with their titles and linked nodes
  $panel_query = "SELECT
  {node}.nid AS nid,
  {node}.title AS page_title,
  {field_data_field_content}.field_content_nid AS link_nid,
  {goldberg_panels}.weight
  FROM {node}
  LEFT JOIN {field_data_field_content} ON {field_data_field_content}.entity_id = {node}.nid
  LEFT JOIN {goldberg_panels} ON {goldberg_panels}.nid = {node}.nid
  WHERE {node}.type = 'panel'
  ORDER BY {goldberg_panels}.weight";

  // Query to get all of the type data for each panel
  $type_query = "SELECT
  {taxonomy_term_data}.tid AS type_id,
  {taxonomy_term_data}.name AS type_name,
  {field_data_field_short_name}.field_short_name_value AS type_short_name
  FROM {field_data_field_type}
  LEFT JOIN {taxonomy_term_data} ON {taxonomy_term_data}.tid = {field_data_field_type}.field_type_tid
  LEFT JOIN {field_data_field_short_name} ON {field_data_field_short_name}.entity_id = {taxonomy_term_data}.tid
  WHERE {field_data_field_type}.entity_id = :nid";

  // Query to get all of the image data for each panel
  $panel_image_query = "SELECT
  {field_data_field_large_panel}.field_large_panel_alt AS large_alt,
  {field_data_field_large_panel}.field_large_panel_width AS large_width,
  {field_data_field_large_panel}.field_large_panel_height AS large_height,
  large_panel.uri AS large_uri,
  {field_data_field_long_panel}.field_long_panel_alt AS long_alt,
  {field_data_field_long_panel}.field_long_panel_width AS long_width,
  {field_data_field_long_panel}.field_long_panel_height AS long_height,
  long_panel.uri AS long_uri,
  {field_data_field_small_panel}.field_small_panel_alt AS small_alt,
  {field_data_field_small_panel}.field_small_panel_width AS small_width,
  {field_data_field_small_panel}.field_small_panel_height AS small_height,
  small_panel.uri AS small_uri,
  {field_data_field_tall_panel}.field_tall_panel_alt AS tall_alt,
  {field_data_field_tall_panel}.field_tall_panel_width AS tall_width,
  {field_data_field_tall_panel}.field_tall_panel_height AS tall_height,
  tall_panel.uri AS tall_uri
  FROM {field_data_field_large_panel}
  LEFT JOIN {field_data_field_long_panel} ON {field_data_field_long_panel}.entity_id = {field_data_field_large_panel}.entity_id
  LEFT JOIN {field_data_field_small_panel} ON {field_data_field_small_panel}.entity_id = {field_data_field_large_panel}.entity_id
  LEFT JOIN {field_data_field_tall_panel} ON {field_data_field_tall_panel}.entity_id = {field_data_field_large_panel}.entity_id
  LEFT JOIN {file_managed} large_panel ON large_panel.fid = {field_data_field_large_panel}.field_large_panel_fid
  LEFT JOIN {file_managed} long_panel ON long_panel.fid = {field_data_field_long_panel}.field_long_panel_fid
  LEFT JOIN {file_managed} small_panel ON small_panel.fid = {field_data_field_small_panel}.field_small_panel_fid
  LEFT JOIN {file_managed} tall_panel ON tall_panel.fid = {field_data_field_tall_panel}.field_tall_panel_fid
  WHERE {field_data_field_large_panel}.entity_id = :nid";

  // Initialize our array to hold the panel data
  $paneldata = array();

  // Get the taxonomy types
  $taxonomy_types = array();
  $taxonomy_types = goldberg_fetch_types();

  // Get the panels
  if (variable_get('goldberg_debug', FALSE) === FALSE) {
    // Setup a cache ID
    $cid = 'GOLDBERG:panel_query_result';
    // Check for a cached version of the query
    if ($cached = cache_get($cid)) {
      // If a cached entry exists, return it
      $panel_query_result = $cached->data;
    }
    else {
      // Run the query
      $panel_query_result = db_query($panel_query)->fetchAll();
      // Cache the query
      cache_set($cid, $panel_query_result, 'cache', strtotime(variable_get('goldberg_cache_duration', '+1 hour')));
    }
  }
  else {
    $panel_query_result = db_query($panel_query)->fetchAll();
  }

  // Start our counter for the panel array
  $panel_counter = 0;
  // Loop over the panels
  foreach ($panel_query_result as $panel_item) {

    $paneldata[$panel_counter]['nid'] = $panel_item->nid;
    $paneldata[$panel_counter]['page_title'] = $panel_item->page_title;
    $paneldata[$panel_counter]['link_nid'] = $panel_item->link_nid;
    $paneldata[$panel_counter]['link_path'] = drupal_lookup_path('alias', "node/" . $panel_item->link_nid);

    // Initialize our array to hold the panel types
    $typesarray = array();

    // Get the types for this panel
    if (variable_get('goldberg_debug', FALSE) === FALSE) {
      // Setup a cache ID
      $cid = 'GOLDBERG:panel_type_result:' . $panel_item->nid;
      // Check for a cached version of the query
      if ($cached = cache_get($cid)) {
        // If a cached entry exists, return it
        $panel_type_result = $cached->data;
      }
      else {
        // Run the query
        $panel_type_result = db_query($type_query, array(':nid' => $panel_item->nid))->fetchAll();
        // Cache the query
        cache_set($cid, $panel_type_result, 'cache', strtotime(variable_get('goldberg_cache_duration', '+1 hour')));
      }
    }
    else {
      $panel_type_result = db_query($type_query, array(':nid' => $panel_item->nid))->fetchAll();
    }

    // Start our counter for the type array
    $type_counter = 0;
    // Loop over the types
    foreach ($panel_type_result as $panel_types) {
      // Look over the all types array and check for the current type
      foreach ($taxonomy_types as $key => $val) {
        // If the shortnames are equal
        if ($val['shortname'] == $panel_types->type_short_name) {
          // Set our color value
          $typesarray[$type_counter]['type_color'] = $taxonomy_types[$key]['color'];
        }
      }

      $typesarray[$type_counter]['type_id'] = $panel_types->type_id;
      $typesarray[$type_counter]['type_name'] = $panel_types->type_name;
      $typesarray[$type_counter]['type_short_name'] = $panel_types->type_short_name;

      // Increment our type counter
      $type_counter++;
    }
    // Insert our type data into our primary array
    $paneldata[$panel_counter]['types'] = $typesarray;

    // Initialize our array to hold the panel images
    $imagesarray = array();

    // Get the images for this panel
    if (variable_get('goldberg_debug', FALSE) === FALSE) {
      // Setup a cache ID
      $cid = 'GOLDBERG:panel_image_result:' . $panel_item->nid;
      // Check for a cached version of the query
      if ($cached = cache_get($cid)) {
        // If a cached entry exists, return it
        $panel_image_result = $cached->data;
      }
      else {
        // Run the query
        $panel_image_result = db_query($panel_image_query, array(':nid' => $panel_item->nid))->fetchAll();
        // Cache the query
        cache_set($cid, $panel_image_result, 'cache', strtotime(variable_get('goldberg_cache_duration', '+1 hour')));
      }
    }
    else {
      $panel_image_result = db_query($panel_image_query, array(':nid' => $panel_item->nid))->fetchAll();
    }

    // Start our counter for the image array
    $image_counter = 0;
    // Loop over the images
    foreach ($panel_image_result as $panel_images) {

      // Populate the large image array
      $imagesarray[$image_counter]['large_image']['alt'] = $panel_images->large_alt;
      $imagesarray[$image_counter]['large_image']['width'] = $panel_images->large_width;
      $imagesarray[$image_counter]['large_image']['height'] = $panel_images->large_height;
      $imagesarray[$image_counter]['large_image']['uri'] = $panel_images->large_uri;

      // Populate the long image array
      $imagesarray[$image_counter]['long_image']['alt'] = $panel_images->long_alt;
      $imagesarray[$image_counter]['long_image']['width'] = $panel_images->long_width;
      $imagesarray[$image_counter]['long_image']['height'] = $panel_images->long_height;
      $imagesarray[$image_counter]['long_image']['uri'] = $panel_images->long_uri;

      // Populate the small image array
      $imagesarray[$image_counter]['small_image']['alt'] = $panel_images->small_alt;
      $imagesarray[$image_counter]['small_image']['width'] = $panel_images->small_width;
      $imagesarray[$image_counter]['small_image']['height'] = $panel_images->small_height;
      $imagesarray[$image_counter]['small_image']['uri'] = $panel_images->small_uri;

      // Populate the tall image array
      $imagesarray[$image_counter]['tall_image']['alt'] = $panel_images->tall_alt;
      $imagesarray[$image_counter]['tall_image']['width'] = $panel_images->tall_width;
      $imagesarray[$image_counter]['tall_image']['height'] = $panel_images->tall_height;
      $imagesarray[$image_counter]['tall_image']['uri'] = $panel_images->tall_uri;

      // Increment our image counter
      $image_counter++;
    }
    // Insert our image data into our primary array
    $paneldata[$panel_counter]['images'] = $imagesarray;

    // Increment our panel counter
    $panel_counter++;
  }

  // Return and array of items
  return $paneldata;
}

/**
 * Get taxonomy types and return an array including colors for front page items
 */
function goldberg_fetch_types() {

  // Build the query
  $term_query = '
    SELECT {taxonomy_term_data}.name, {taxonomy_term_data}.weight, {field_data_field_short_name}.field_short_name_value AS shortname
    FROM {taxonomy_term_data}
    LEFT JOIN {taxonomy_vocabulary} ON {taxonomy_vocabulary}.vid = {taxonomy_term_data}.vid
    LEFT JOIN {field_data_field_short_name} ON {field_data_field_short_name}.entity_id = {taxonomy_term_data}.tid
    WHERE {taxonomy_vocabulary}.name = :type
    ORDER BY {taxonomy_term_data}.weight ASC
  ';

  if (variable_get('goldberg_debug', FALSE) === FALSE) {
    // Setup a cache ID
    $cid = 'GOLDBERG:term_query_result';
    // Check for a cached version of the query
    if ($cached = cache_get($cid)) {
      // If a cached entry exists, return it
      $term_query_result = $cached->data;
    }
    else {
      // Run the query
      $term_query_result = db_query($term_query, array(':type' => 'Type'))->fetchAll();
      // Cache the query
      cache_set($cid, $term_query_result, 'cache', strtotime(variable_get('goldberg_cache_duration', '+1 hour')));
    }
  }
  else {
    $term_query_result = db_query($term_query, array(':type' => 'Type'))->fetchAll();
  }


  // Run the query
  $term_query_result = db_query($term_query, array(':type' => 'Type'));

  // Initialize the return array
  $types = array();
  // Initialize a counter
  $counter = 0;
  // Get the number of types outside of the first three
  $extra_types = $term_query_result->rowCount() - 3;
  // Set the starting RGB value (white)
  $current = 255;

  // Loop over the types
  foreach ($term_query_result as $item) {

    // If we are past the first 3 counters, start our color gradient
    if ($counter > 2) {
      // Calculate the interval that we will count by to create our shades
      $interval = (255 - 140) / $extra_types;
      // Create the current color
      $color = $current . ', ' . $current . ', ' . $current;

      /**
       * The color can be overridden here, the following example would give green as a base starting color
       * $color = '0, ' . $current . ', 0';
       */

      // Update our current shade for the next round
      $current = round($current - $interval);
    }
    else {
      // Assign the color for the first three elements (black, red, grey)
      switch ($counter) {
        case 0:
          // Black
          $color = "0, 0, 0";
        break;
        case 1:
          // Red
          $color = "153, 0, 0";
        break;
        case 2:
          // Grey
          $color = "153, 153, 153";
        break;
      }
    }

    // Populate the array
    $types[$counter]['id'] = $counter;
    $types[$counter]['name'] = $item->name;
    $types[$counter]['shortname'] = $item->shortname;
    $types[$counter]['color'] = $color;

    // Increment the counter
    $counter++;
  }

  // Return the types array
  return $types;
}