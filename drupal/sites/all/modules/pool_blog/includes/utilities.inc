<?php

/**
 * @file
 * Helper function for the Pool Blog Customizations.
 */

/**
 * Custom submit form for the blog add form.
 */
function pool_blog_blog_node_submit(&$form, &$form_state) {
  // dd($form_state['values'], 'pool_values_before');
  if (isset($form_state['values']['word_doc']) && !empty($form_state['values']['word_doc'])) {
    $fid = $form_state['values']['word_doc'];
    $file = file_load($fid);

    if (isset($file) && $file) {
      $uri = $file->uri;
      $source = drupal_realpath($uri);
      $destination = drupal_realpath('temporary://');

      $converted_file = pool_blog_convert_file($source, $destination, 'html', $file->filename);

      if ($converted_file) {
        // dd($converted_file, 'processed');
        $contents = file_get_contents("$destination/$converted_file");
        $open = strpos($contents, '<body');
        $x = substr($contents, $open);
        $x = preg_replace('/\<body.*>/', '', $x);
        $x = preg_replace('/\<\/body.*>/', '', $x);
        $x = preg_replace('/\<\/html.*>/', '', $x);
        $x = preg_replace('/\r|\n/', '', $x);

        //$form['body']['und'][0]['#default_value'] = $x;
        // dd($x, 'pool_blog_content');
        $form_state['values']['body']['und'][0]['value'] = $x;
        $form_state['values']['body']['und'][0]['format'] = 'full_html';
          // dd($form_state['values'], 'form state on submit');
      }

      file_delete($file);
    }
  }
}

function pool_blog_convert_file($source = NULL, $dest_dir = NULL, $format = 'html', $filename = NULL) {
  if (isset($source)) {

    switch ($format) {
      case 'html':
        //$command = "soffice --headless --convert-to $format $source --outdir $dest_dir";
        return pool_blog_convert_file_to_html($source, $dest_dir, $format, $filename);
        break;
      default:
    }
  }
}

/**
 * Convert the doc/docx file to html.
 */
function pool_blog_convert_file_to_html($source, $dest_dir, $format, $filename = NULL) {
  $html_extension = ".html";
  $command = "export HOME=/tmp; /usr/bin/soffice --headless --convert-to $format '$source' --outdir '$dest_dir'";
  $converted_filename = '';
  //dd($command, 'pool_blog_command');
  exec($command, $output, $return_var);
  // system($command, $return_var);

  // dd($output, 'pool_output');
  // dd($return_var, 'pool_return');

  $filename_without_extension = substr($filename, 0, strpos($filename, "."));
  $converted_filename = $filename_without_extension . $html_extension;
  // dd($dest_dir . "/$converted_filename", 'name1');
  if ($converted_filename && file_exists($dest_dir . "/$converted_filename")) {
  // if ($converted_filename && file_exists("/tmp/$converted_filename/$converted_filename")) {
    // dd($converted_filename, 'pool_blog_converted_file');
    return $converted_filename;
  }
  else {
    watchdog(WATCHDOG_ALERT, t('Failed to convert file.'));
    return '';
  }
}
