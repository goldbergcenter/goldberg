<?php

/**
 * @file
 * Function call to populate the panel navigation on the front page
 */

/**
 * Display the panel navigation
 */
function goldberg_panelnav_populate() {

  // Initialize the output variable
  $output = '';

  // Display the menu items
  $output .= '<ul id="modifiers-list">' . PHP_EOL;
    $output .= '  <li class="modify-item filters">' . PHP_EOL;
    $output .= '    <a class="modify-item-link hovertitle" title="show all" href="#" data-filter="*">' . PHP_EOL;
    $output .= '      <p class="all-type"><span class="element-invisible element-focusable">show all</span></p>' . PHP_EOL;
    $output .= '    </a>' . PHP_EOL;
    $output .= '  </li>' . PHP_EOL;

  // Get the types
  $types = array();
  $types = goldberg_fetch_types();

  // Loop over the types
  for ($i = 0; $i < count($types); $i++) {
    $output .= '  <li class="modify-item filters">' . PHP_EOL;
    $output .= '    <a class="modify-item-link hovertitle" title="' . $types[$i]['name'] . '" href="#" data-filter=".' . $types[$i]['shortname'] . '">' . PHP_EOL;
    $output .= '      <p class="item-type" item-' . $types[$i]['id'] . '-type" style="background-color: rgb(' . $types[$i]['color'] . ')"><span class="element-invisible element-focusable">' . $types[$i]['name'] . '</span></p>' . PHP_EOL;
    $output .= '    </a>' . PHP_EOL;
    $output .= '  </li>' . PHP_EOL;
  }

    $output .= '  <li class="modify-item sort-by">' . PHP_EOL;
    $output .= '    <a class="modify-item-link hovertitle" title="sort by default" href="#default">' . PHP_EOL;
    $output .= '      <p class="sort-by-default"><span class="element-invisible element-focusable">Default</span></p>' . PHP_EOL;
    $output .= '    </a>' . PHP_EOL;
    $output .= '  </li>' . PHP_EOL;
    $output .= '  <li class="modify-item sort-by">' . PHP_EOL;
    $output .= '    <a class="modify-item-link hovertitle" title="sort by title" href="#title">' . PHP_EOL;
    $output .= '      <p class="sort-by-title"><span class="element-invisible element-focusable">Title</span></p>' . PHP_EOL;
    $output .= '    </a>' . PHP_EOL;
    $output .= '  </li>' . PHP_EOL;
    $output .= '  <li class="modify-item sort-by">' . PHP_EOL;
    $output .= '    <a class="modify-item-link hovertitle" title="sort by type" href="#type">' . PHP_EOL;
    $output .= '      <p class="sort-by-type"><span class="element-invisible element-focusable">Type</span></p>' . PHP_EOL;
    $output .= '    </a>' . PHP_EOL;
    $output .= '  </li>' . PHP_EOL;
    $output .= '  <li class="modify-item toggle-sizes">' . PHP_EOL;
    $output .= '    <a class="modify-item-link hovertitle" title="toggle sizes" href="#toggle-sizes">' . PHP_EOL;
    $output .= '      <p class="toggle-sizes-item"><span class="element-invisible element-focusable">Toggle Sizes</span></p>' . PHP_EOL;
    $output .= '    </a>' . PHP_EOL;
    $output .= '  </li>' . PHP_EOL;
  $output .= '</ul>' . PHP_EOL;

  // Return the output
  return $output;
}