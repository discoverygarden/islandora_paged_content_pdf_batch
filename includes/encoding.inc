<?php
/**
 * @file
 * Helper functions to help with string encoding issues.
 */

/**
 * Helper to create a form element to allow an encoding to selected.
 *
 * Provides the options as they should be used in drupal_convert_to_utf8().
 *
 * @return array
 *   An array representing the parameters for a form element.
 */
function islandora_paged_content_pdf_batch_get_encoding_element() {
  $options = islandora_paged_content_pdf_batch_get_encoding_options();

  $base = array(
    '#title' => t('ZIP File Name Encoding'),
    '#description' => t('The encoding of files containing in the ZIP file.'),
    '#default_value' => NULL,
  );
  if ($options === NULL) {
    $base['#type'] = 'textfield';
  }
  elseif (is_array($options)) {
    $base += array(
      '#type' => 'select',
      '#empty_option' => t('Default Encoding'),
      '#options' => drupal_map_assoc($options),
    );
  }
  elseif ($options === FALSE) {
    $base += array(
      '#type' => 'value',
      '#value' => NULL,
    );
  }

  return $base;
}

/**
 * Helper to get the encodings processable by drupal_convert_to_utf8().
 *
 * @return array|NULL|FALSE
 *   An array of encodings if they could be detected. NULL if we have an
 *   encoding mechanism, but no means to determine the available encodings;
 *   otherwise boolean FALSE if we have no mechanism with which to perform an
 *   encoding (no iconv, mbstring or GNU recode available).
 */
function islandora_paged_content_pdf_batch_get_encoding_options() {
  if (function_exists('iconv')) {
    $output = array();
    $return = 0;
    exec('iconv -l', $output, $return);
    if ($return !== 0) {
      watchdog('islandora_paged_content_pdf_batch', 'Unable to determine list of encodings supported by iconv.');
      return NULL;
    }
    else {
      // XXX: "iconv -l" provides listing with the encoding names follows by a
      // slash, an optional alias (?) and an final slash.
      return preg_replace('/^([^\/]+).*$/', '\\1', array_map('trim', $output));
    }
  }
  elseif (function_exists('mb_convert_encoding')) {
    return mb_list_encodings();
  }
  elseif (function_exists('recode_string')) {
    // Unknown... No way to get what is availalbe.
    return NULL;
  }
  else {
    watchdog('islandora_paged_content_pdf_batch', 'No mechanism to convert encodings.');
    return FALSE;
  }
}
