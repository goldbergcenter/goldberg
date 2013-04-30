<?php

/**
 * @file
 * Primary function calls for the Goldberg front page
 */

/**
 * Display the front page panel
 */
function goldberg_panel_display() {

  // Define the output variable for our content
  $output = '';

  // Get the valid information from the database
  $paneldata = goldberg_panel_fetchdata();

  // Populate our items
  $output .= goldberg_panel_populate($paneldata);

  // Return the output
  return $output;
}

/**
 * Populate the items with content
 */
function goldberg_panel_populate($paneldata) {

  // Define the output variable for our content
  $output = '';

  // Loop over our panels
  for($p = 0; $p < count($paneldata); $p++) {

    // Initialize an array to hold our type short names
    $typearray = array();
    // Initialize a variable to hold our list of item types
    $typelist = '';
    // Loop over the types
    for($t = 0; $t < count($paneldata[$p]['types']); $t++) {
      // Store the short names in our array
      $typearray[] = $paneldata[$p]['types'][$t]['type_short_name'];
      // Add types to our list
      $typelist .= '
      <li class="type ' . $paneldata[$p]['types'][$t]['type_short_name'] . '-icon">
        <span class="element-invisible element-focusable">' . $paneldata[$p]['types'][$t]['type_name'] . '</span>
      </li>';
    }
    // Convert our type short name array into a space seperated list to be used as classes
    $short_name_items = implode(" ", $typearray);


    // Start the item and provide the type classes for this item
    $output .= '<div class="item ' . $short_name_items . ' itemnum' . $p . '" itemnum="' . $p . '"><a class="item-link" href="' . $paneldata[$p]['link_path'] . '" title="' . $paneldata[$p]['page_title'] . '">';

    // Number the item
    $output .= '<span class="number element-invisible element-focusable">' . $p . '</span>';

    // Display the item title
    $output .= '<p class="title">' . $paneldata[$p]['page_title'] . '</p>';

    // Loop over the images
    for($i = 0; $i < count($paneldata[$p]['images']); $i++) {

      // Generate and style our image
      $small_panel = array(
        'style_name' => 'small-panel',
        'path' => $paneldata[$p]['images'][$i]['small_image']['uri'],
        'alt' => $paneldata[$p]['images'][$i]['small_image']['alt'],
        'width' => $paneldata[$p]['images'][$i]['small_image']['width'],
        'height' => $paneldata[$p]['images'][$i]['small_image']['height'],
        'attributes' => array('class' => 'item-image small-panel hide-panel'),
      );
      $large_panel = array(
        'style_name' => 'large-panel',
        'path' => $paneldata[$p]['images'][$i]['large_image']['uri'],
        'alt' => $paneldata[$p]['images'][$i]['large_image']['alt'],
        'width' => $paneldata[$p]['images'][$i]['large_image']['width'],
        'height' => $paneldata[$p]['images'][$i]['large_image']['height'],
        'attributes' => array('class' => 'item-image large-panel hide-panel'),
      );
      $tall_panel = array(
        'style_name' => 'tall-panel',
        'path' => $paneldata[$p]['images'][$i]['tall_image']['uri'],
        'alt' => $paneldata[$p]['images'][$i]['tall_image']['alt'],
        'width' => $paneldata[$p]['images'][$i]['tall_image']['width'],
        'height' => $paneldata[$p]['images'][$i]['tall_image']['height'],
        'attributes' => array('class' => 'item-image tall-panel hide-panel'),
      );
      $long_panel = array(
        'style_name' => 'long-panel',
        'path' => $paneldata[$p]['images'][$i]['long_image']['uri'],
        'alt' => $paneldata[$p]['images'][$i]['long_image']['alt'],
        'width' => $paneldata[$p]['images'][$i]['long_image']['width'],
        'height' => $paneldata[$p]['images'][$i]['long_image']['height'],
        'attributes' => array('class' => 'item-image long-panel hide-panel'),
      );
      // Render the link
      $small_image_url = theme('image_style', $small_panel);
      $large_image_url = theme('image_style', $large_panel);
      $tall_image_url = theme('image_style', $tall_panel);
      $long_image_url = theme('image_style', $long_panel);
      $output .= $small_image_url;
      $output .= $large_image_url;
      $output .= $tall_image_url;
      $output .= $long_image_url;
    }

    // display the list of types this item belongs to
    $output .= '<ul class="types">';

    // display the list items from the variable created in our types loop
    $output .= $typelist;

    // close the list and div
    $output .= '</ul>';
    $output .= '</a></div>';

  }

  // Return the output
  return $output;
}

/**
 * Get content for the panel from the database
 */
function goldberg_panel_fetchdata() {

  // Query to get all panels with their titles and linked nodes
  $panel_query = "SELECT
  node.nid AS nid,
  node.title AS page_title,
  field_data_field_content.field_content_nid AS link_nid
  FROM node
  LEFT JOIN field_data_field_content ON field_data_field_content.entity_id = node.nid
  WHERE node.type = 'panel'";

  // Query to get all of the type data for each panel
  $type_query = "SELECT
  taxonomy_term_data.tid AS type_id,
  taxonomy_term_data.name AS type_name,
  field_data_field_short_name.field_short_name_value AS type_short_name
  FROM field_data_field_type
  LEFT JOIN taxonomy_term_data ON taxonomy_term_data.tid = field_data_field_type.field_type_tid
  LEFT JOIN field_data_field_short_name ON field_data_field_short_name.entity_id = taxonomy_term_data.tid
  WHERE field_data_field_type.entity_id = :nid";

  // Query to get all of the image data for each panel
  $panel_image_query = "SELECT
  field_data_field_large_panel.field_large_panel_alt AS large_alt,
  field_data_field_large_panel.field_large_panel_width AS large_width,
  field_data_field_large_panel.field_large_panel_height AS large_height,
  large_panel.uri AS large_uri,
  field_data_field_long_panel.field_long_panel_alt AS long_alt,
  field_data_field_long_panel.field_long_panel_width AS long_width,
  field_data_field_long_panel.field_long_panel_height AS long_height,
  long_panel.uri AS long_uri,
  field_data_field_small_panel.field_small_panel_alt AS small_alt,
  field_data_field_small_panel.field_small_panel_width AS small_width,
  field_data_field_small_panel.field_small_panel_height AS small_height,
  small_panel.uri AS small_uri,
  field_data_field_tall_panel.field_tall_panel_alt AS tall_alt,
  field_data_field_tall_panel.field_tall_panel_width AS tall_width,
  field_data_field_tall_panel.field_tall_panel_height AS tall_height,
  tall_panel.uri AS tall_uri
  FROM field_data_field_large_panel
  LEFT JOIN field_data_field_long_panel ON field_data_field_long_panel.entity_id = field_data_field_large_panel.entity_id
  LEFT JOIN field_data_field_small_panel ON field_data_field_small_panel.entity_id = field_data_field_large_panel.entity_id
  LEFT JOIN field_data_field_tall_panel ON field_data_field_tall_panel.entity_id = field_data_field_large_panel.entity_id
  LEFT JOIN file_managed large_panel ON large_panel.fid = field_data_field_large_panel.field_large_panel_fid
  LEFT JOIN file_managed long_panel ON long_panel.fid = field_data_field_long_panel.field_long_panel_fid
  LEFT JOIN file_managed small_panel ON small_panel.fid = field_data_field_small_panel.field_small_panel_fid
  LEFT JOIN file_managed tall_panel ON tall_panel.fid = field_data_field_tall_panel.field_tall_panel_fid
  WHERE field_data_field_large_panel.entity_id = :nid";

  // Initialize our array to hold the panel data
  $paneldata = array();

  // Get the panels
  $panel_query_result = db_query($panel_query);
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
    $panel_type_result = db_query($type_query, array(':nid' => $panel_item->nid));
    // Start our counter for the type array
    $type_counter = 0;
    // Loop over the types
    foreach ($panel_type_result as $panel_types) {

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
    $panel_image_result = db_query($panel_image_query, array(':nid' => $panel_item->nid));
    // Start our counter for the image array
    $image_counter = 0;
    // Loop over the images
    foreach ($panel_image_result as $panel_images) {

      // Populate the large image array
      $imagesarray[$image_counter]['large_image']['alt'] = $panel_images->large_alt;
      $imagesarray[$image_counter]['large_image']['width'] = $panel_images->large_width;
      $imagesarray[$image_counter]['large_image']['height'] = $panel_images->long_height;
      $imagesarray[$image_counter]['large_image']['uri'] = $panel_images->long_uri;

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