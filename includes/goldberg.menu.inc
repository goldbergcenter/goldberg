<?php

/**
 * @file
 * Function calls to populate the dropdown menu on all pages
 */

/**
 * Display the front page panel
 */
function goldberg_menu_populate() {

  // Define the output variable for our content
  $output = '';

  // Get the valid panel information from the database
  $paneldata = goldberg_panel_fetchdata();

  // Define an array to hold our links in
  $link_array = array();

  // Initialize a panel counter
  $panel_counter = 0;
  // Loop over our panels
  for ($p = 0; $p < count($paneldata); $p++) {
    // Loop over the types for each panel
    for ($t = 0; $t < count($paneldata[$p]['types']); $t++) {
      $link_array[$panel_counter]['title'] = $paneldata[$p]['page_title'];
      $link_array[$panel_counter]['link'] = $paneldata[$p]['link_path'];
      $link_array[$panel_counter]['type'] = $paneldata[$p]['types'][$t]['type_name'];
      // Increment our counter
      $panel_counter++;
    }
  }

  // Create a temporary array to sort with
  $tempsorter = array();
  // Create a temporary array to store our completed values in
  $final_array = array();

  // Reset the index of our array
  reset($link_array);

  // Grab all of the types and put them into an array
  foreach ($link_array as $tempkey => $typearray) {
    $tempsorter[$tempkey] = $typearray['type'];
  }

  // Sort all of the types alphabetically
  asort($tempsorter);

  // Put the newly sorted array back into the main array
  foreach ($tempsorter as $ii => $typearray) {
    $final_array[] = $link_array[$ii];
  }

  // Write the finalized sorted array back to our original value
  $temp_array = $final_array;
  // Create a temporary container for all pages
  $page_holder = array();
  // Create a holder for the array keys that point to pages
  $page_keys = array();

  // Loop over the new temporary array so we can store page keys
  for ($i=0; $i < count($temp_array); $i++) {
    // If the current element is a page
    if ($temp_array[$i]['type'] == 'Page') {
      // Store the key in our page key array
      $page_keys[] = $i;
    }
  }

  // Loop over our pages and remove them from the temporary array
  for ($i=0; $i < count($page_keys); $i++) {
    // Define our current key
    $current_key = $page_keys[$i];
    // Store the element we are about to remove, so we can add it back later
    $page_holder[] = $temp_array[$current_key];
    unset($temp_array[$current_key]);
  }

  // Initialize an array to hold the rekeyed values
  $clean_array = array();
  // Rekey the array
  foreach ($temp_array as $temp_element) {
    // Accounting for our removed items, we need to rekey the array
    $clean_array[] = $temp_element;
  }

  // Put the pages back into the menu array
  for ($i=0; $i < count($page_holder); $i++) {
    // Put the current page at the beginning of the array
    array_unshift($clean_array , $page_holder[$i]);
  }

  // Write the finalized sorted array back to our original value
  $link_array = $clean_array;

  // Clean up our array
  reset($link_array);

  // Create a variable to store the last item looped over into
  $last_title = '';
  // Loop over our array to build the menu
  for ($p = 0; $p < count($link_array); $p++) {

    // Store the current type we are looping over (helps us create a header)
    $current_title = $link_array[$p]['type'];

    // If this is a new type
    if ($last_title != $current_title) {

      // Store our current type in the last type variable so we won't land here again next loop if we are still under the same type
      $last_title = $link_array[$p]['type'];

      // If we are not on the first loop, close the list and div container for the menu group
      if ($p != 0) {
        $output .= '</ul></div>';
      }

      // Open the div container for the menu group
      $output .= '<div class="gb-menu-panel-menu-column">';

      // If we are not on a page, display the title of the section
      if ($last_title != 'Page') {
        $output .= '<h3>' . $last_title . '</h3>';
      }
      else {
        $output .= '<h3>Main</h3>';
      }
      // Open our list
      $output .= '<ul>';

    }

    // Display our list item
    $output .= '<li><a href="' . $link_array[$p]['link'] . '" title="' . $link_array[$p]['title'] . '">' . $link_array[$p]['title'] . '</a></li>';

    // If we are on the last element in the array, close the list and div container for the menu group
    if ($p == count($link_array) - 1) {
      $output .= '</ul></div>';
    }

  }

  // Return the output
  return $output;
}

