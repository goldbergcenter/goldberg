<?php
/**
 * @file
 * Admin page callbacks for the Pathauto module.
 *
 * @ingroup goldberg
 */

/**
 * Form builder; Order the front page panels.
 *
 * @ingroup forms
 * @see system_settings_form()
 */
function goldberg_frontpage_form($form, $form_state) {
  return system_settings_form($form);
}