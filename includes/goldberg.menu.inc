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
  $linkArray = array();

  // Initialize a panel counter
  $panelCounter = 0;
  // Loop over our panels
  for($p = 0; $p < count($paneldata); $p++) {
    // Loop over the types for each panel
    for($t = 0; $t < count($paneldata[$p]['types']); $t++) {
      $linkArray[$panelCounter]['title'] = $paneldata[$p]['page_title'];
      $linkArray[$panelCounter]['link'] = $paneldata[$p]['link_path'];
      $linkArray[$panelCounter]['type'] = $paneldata[$p]['types'][$t]['type_name'];
      // Increment our counter
      $panelCounter++;
    }
  }

  // Create a temporary array to sort with
  $tempsorter = array();
  // Create a temporary array to store our completed values in
  $finalArray = array();

  // Reset the index of our array
  reset($linkArray);

  // Grab all of the types and put them into an array
  foreach ($linkArray as $tempkey => $typearray) {
    $tempsorter[$tempkey] = $typearray['type'];
  }

  // Sort all of the types alphabetically
  asort($tempsorter);

  // Put the newly sorted array back into the main array
  foreach ($tempsorter as $ii => $typearray) {
    $finalArray[] = $linkArray[$ii];
  }

  // Write the finalized sorted array back to our original value
  $tempArray = $finalArray;
  // Create a temporary container for all pages
  $pageHolder = array();
  // Create a holder for the array keys that point to pages
  $pageKeys = array();

  // Loop over the new temporary array so we can store page keys
  for ($i=0; $i < count($tempArray); $i++) {
    // If the current element is a page
    if ($tempArray[$i]['type'] == 'Page') {
      // Store the key in our page key array
      $pageKeys[] = $i;
    }
  }

  // Loop over our pages and remove them from the temporary array
  for ($i=0; $i < count($pageKeys); $i++) {
    // Define our current key
    $currentKey = $pageKeys[$i];
    // Store the element we are about to remove, so we can add it back later
    $pageHolder[] = $tempArray[$currentKey];
    unset($tempArray[$currentKey]);
  }

  // Initialize an array to hold the rekeyed values
  $cleanArray = array();
  // Rekey the array
  foreach ($tempArray as $tempElement) {
    // Accounting for our removed items, we need to rekey the array
    $cleanArray[] = $tempElement;
  }

  // Put the pages back into the menu array
  for ($i=0; $i < count($pageHolder); $i++) {
    // Put the current page at the beginning of the array
    array_unshift($cleanArray , $pageHolder[$i]);
  }

  // Write the finalized sorted array back to our original value
  $linkArray = $cleanArray;

  // Clean up our array
  reset($linkArray);

  // Create a variable to store the last item looped over into
  $lastTitle = '';
  // Loop over our array to build the menu
  for($p = 0; $p < count($linkArray); $p++) {

    // Store the current type we are looping over (helps us create a header)
    $currentTitle = $linkArray[$p]['type'];

    // If this is a new type
    if ($lastTitle != $currentTitle) {

      // Store our current type in the last type variable so we won't land here again next loop if we are still under the same type
      $lastTitle = $linkArray[$p]['type'];

      // If we are not on the first loop, close the list and div container for the menu group
      if ($p != 0) {
        $output .= '</ul></div>';
      }

      // Open the div container for the menu group
      $output .= '<div class="gb-menu-panel-menu-column">';

      // If we are not on a page, display the title of the section
      if ($lastTitle != 'Page') {
        $output .= '<h3>' . $lastTitle . '</h3>';
      }
      else {
        $output .= '<h3>Main</h3>';
      }
      // Open our list
      $output .= '<ul>';

    }

    // Display our list item
    $output .= '<li><a href="' . $linkArray[$p]['link'] . '" title="' . $linkArray[$p]['title'] . '">' . $linkArray[$p]['title'] . '</a></li>';

    // If we are on the last element in the array, close the list and div container for the menu group
    if($p == count($linkArray) - 1) {
      $output .= '</ul></div>';
    }

  }

  // Return the output
  return $output;
}

