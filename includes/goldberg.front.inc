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

  // Get the valid panel information from the database
  $paneldata = goldberg_panel_fetchdata();

  $output .= '<div id="gb-front-container">';

  // Populate our items
  $output .= goldberg_panel_populate($paneldata);

  $output .= '</div>';

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
  for ($p = 0; $p < count($paneldata); $p++) {

    // Initialize an array to hold our type short names
    $typearray = array();
    // Initialize a variable to hold our list of item types
    $typelist = '';
    // Loop over the types
    for ($t = 0; $t < count($paneldata[$p]['types']); $t++) {
      // Store the short names in our array
      $typearray[] = $paneldata[$p]['types'][$t]['type_short_name'];
      // Add types to our list
      $typelist .= '
      <li class="type type-icon" style="background-color: rgb(' . $paneldata[$p]['types'][$t]['type_color'] . ')">
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
    for ($i = 0; $i < count($paneldata[$p]['images']); $i++) {

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