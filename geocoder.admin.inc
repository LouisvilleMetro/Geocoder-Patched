<?php

/**
 * @file
 * Settings form.
 */

/**
 * Module settings page.
 */
function geocoder_admin_settings($form, &$form_state) {
  $geocoder_settings= variable_get("geocoder_settings", array());
  $form['geocoder_apikey_yahoo'] = array(
    '#type' => 'textfield',
    '#title' => t('Yahoo PlaceFinder API Key'),
    '#description' => t('You can obtain a Yahoo PlaceFinder consumer key at') . ' ' . 'http://developer.yahoo.com/geo/placefinder/',
    '#default_value' => empty($geocoder_settings['geocoder_apikey_yahoo']) ? '' : $geocoder_settings['geocoder_apikey_yahoo'],
    '#required' => FALSE,
  );

  $form['geocoder_apikey_yandex'] = array(
    '#type' => 'textfield',
    '#title' => t('Yandex Maps API Key'),
    '#description' => t('You can obtain a Yandex API Key at ') . 'http://api.yandex.ru/maps/getkey.xml',
    '#default_value' => empty($geocoder_settings['geocoder_apikey_yandex']) ? '' : $geocoder_settings['geocoder_apikey_yandex'],
    '#required' => FALSE,
  );

  $form['geocoder_apikey_bing'] = array(
    '#type' => 'textfield',
    '#title' => t('Bing API Key'),
    '#description' => t('You can obtain a Bing API Key at ') . 'http://msdn.microsoft.com/en-us/library/ff428642.aspx',
    '#default_value' => empty($geocoder_settings['geocoder_apikey_bing']) ? '' : $geocoder_settings['geocoder_apikey_bing'],
    '#required' => FALSE,
  );

  $form['geocoder_apikey_mapzen'] = array(
    '#type' => 'textfield',
    '#title' => t('Mapzen Search API Key'),
    '#description' => t('You can obtain a Mapzen Search API Key at ') . 'https://mapzen.com/developers',
    '#default_value' => empty($geocoder_settings['geocoder_apikey_mapzen']) ? '' : $geocoder_settings['geocoder_apikey_mapzen'],
    '#required' => FALSE,
  );

  $form['geocoder_google_auth_method'] = array(
    '#type'          => 'select',
    '#title'         => t('Google API Authorization Method'),
    '#description'   => t("If your website runs on shared hosting, you'll want to authenticate requests to the Google Geocoding API to reduce the likelihood of being rate limited (2500 requests per day / 5 requests per second). Alternatively, Google Maps for Work customers may use their Client ID and Signing Key to authenticate."),
    '#default_value' => variable_get('geocoder_google_auth_method', GEOCODER_GOOGLE_AUTH_NONE),
    '#options'       => array(
      GEOCODER_GOOGLE_AUTH_NONE => 'None',
      GEOCODER_GOOGLE_AUTH_KEY  => 'API Key (free)',
      GEOCODER_GOOGLE_AUTH_WORK => 'Google Maps API for Work',
    ),
  );

  $form['geocoder_apikey_google'] = array(
    '#type'        => 'textfield',
    '#title'       => t('Google Maps API Key'),
    '#description' => t('Obtain a free Google Geocoding API Key at <a href="@link">@link</a>', array(
      '@link' => 'https://developers.google.com/maps/documentation/geocoding/#api_key',
    )),
    '#default_value' => empty($geocoder_settings['geocoder_apikey_google']) ? '' : $geocoder_settings['geocoder_apikey_google'],
    '#required'      => FALSE,
    '#states'        => array(
      'visible' => array(
        ':input[name="geocoder_google_auth_method"]' => array('value' => GEOCODER_GOOGLE_AUTH_KEY),
      ),
    ),
  );

  $form['geocoder_google_client_id'] = array(
    '#type'        => 'textfield',
    '#title'       => t('Google Maps API for Work: Client ID'),
    '#description' => t('For more information, visit <a href="@link">@link</a>', array(
      '@link' => 'https://developers.google.com/maps/documentation/business/webservices/auth#business-specific_parameters',
    )),
    '#default_value' => variable_get('geocoder_google_client_id'),
    '#required'      => FALSE,
    '#states'        => array(
      'visible' => array(
        ':input[name="geocoder_google_auth_method"]' => array(
          'value' => GEOCODER_GOOGLE_AUTH_WORK,
        ),
      ),
    ),
  );

  $form['geocoder_google_private_key'] = array(
    '#type'        => 'textfield',
    '#title'       => t('Google Maps API for Work: Private/Signing Key'),
    '#description' => t('For more information, visit <a href="@link">@link</a>', array(
      '@link' => 'https://developers.google.com/maps/documentation/business/webservices/auth#how_do_i_get_my_signing_key',
    )),
    '#default_value' => variable_get('geocoder_google_private_key'),
    '#required'      => FALSE,
    '#states'        => array(
      'visible' => array(
        ':input[name="geocoder_google_auth_method"]' => array(
          'value' => GEOCODER_GOOGLE_AUTH_WORK,
        ),
      ),
    ),
  );

  $form['geocoder_google_delay'] = array(
    '#type' => 'textfield',
    '#title' => t('Delay between Google geocoding requests (in milliseconds)'),
    '#description' => t('Adds a delay between geocoding requests, to avoid OVER_QUERY_LIMIT errors from Google. 200ms is recommended.'),
    '#default_value' => variable_get('geocoder_google_delay', 0),
    '#size' => 10,
  );

  $form['geocoder_cache_empty_results'] = array(
    '#type' => 'checkbox',
    '#title' => t('Cache empty results'),
    '#default_value' => variable_get('geocoder_cache_empty_results', TRUE),
    '#description' => t('Geocoder caches all queries by default. Do you want that to include the ones with no results? If not, it will be checked every time, probably with the same no-result.'),
  );

  geophp_load();
  $month_utc = strtotime('-1 month');
  $week_utc = strtotime('-1 week');
  $entries = db_query('SELECT * FROM {cache_geocoder}');
  $empty = $non_empty = 0;
  $month = $week = 0;
  foreach ($entries as $entry) {
    $data = unserialize($entry->data);

    $data['geometry'] ? $non_empty++ : $empty++;

    if ($entry->created > $month_utc) {
      $month++;
    }
    if ($entry->created > $week_utc) {
      $week++;
    }
  }

  $form['cache'] = array(
    '#type' => 'fieldset',
    '#title' => t('Cache (!total)', array('!total' => $empty + $non_empty)),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $presuffix = array(
    '#prefix' => '<p>',
    '#suffix' => '</p>',
  );
  $form['cache']['emptiness'] = array(
    '#markup' => t('!non_empty entries have results, and !empty have no results.', array(
      '!non_empty' => $non_empty,
      '!empty' => $empty,
    )),
  ) + $presuffix;
  $form['cache']['dated'] = array(
    '#markup' => t('!month entries are from the last month, !week of those are from the last week.', array(
      '!month' => $month,
      '!week' => $week,
    )),
  ) + $presuffix;

  $form['cache']['clear'] = array(
    '#type' => 'container',
  );
  $form['cache']['clear']['all'] = array(
    '#type' => 'submit',
    '#value' => t('Clear ALL cache'),
    '#submit' => array('geocoder_admin_settings_clear_cache_all_submit'),
  );
  $form['cache']['clear']['empty'] = array(
    '#type' => 'submit',
    '#value' => t('Clear all NO-RESULTS cache'),
    '#submit' => array('geocoder_admin_settings_clear_cache_noresults_submit'),
  );

  $form['#submit'][] = 'geocoder_admin_settings_submit';
  return system_settings_form($form);
}

/**
 * Validation callback for geocoder_admin_settings().
 */
function geocoder_admin_settings_validate($form_id, $form_values) {
  if (!empty($form_values['values']['geocoder_apikey_yahoo']) && preg_match("/[^A-Za-z0-9\\-]/", trim($form_values['values']['geocoder_apikey_yahoo']))) {
    form_set_error('geocoder_apikey_yahoo', t('Yahoo API keys are alpha numeric and dashes only.'));
  }
}

/**
 * Submit callback for geocoder_admin_settings().
 */
function geocoder_admin_settings_submit($form, &$form_state) {
  $geocoder_settings= variable_get("geocoder_settings", array());
  $geocoder_settings['geocoder_apikey_yahoo'] = trim($form_state['values']['geocoder_apikey_yahoo']);
  $geocoder_settings['geocoder_apikey_yandex'] = trim($form_state['values']['geocoder_apikey_yandex']);
  $geocoder_settings['geocoder_apikey_bing'] = trim($form_state['values']['geocoder_apikey_bing']);
  $geocoder_settings['geocoder_apikey_mapzen'] = trim($form_state['values']['geocoder_apikey_mapzen']);
  $geocoder_settings['geocoder_apikey_google'] = trim($form_state['values']['geocoder_apikey_google']);
  variable_set("geocoder_settings", $geocoder_settings);
}

/**
 * Clear cache (ALL) submit handler from geocoder_admin_settings().
 */
function geocoder_admin_settings_clear_cache_all_submit($form, &$form_state) {
  cache_clear_all('*', 'cache_geocoder', TRUE);

  drupal_set_message(t('All Geocoder cache cleared.'));
}

/**
 * Clear cache (EMPTY) submit handler from geocoder_admin_settings().
 */
function geocoder_admin_settings_clear_cache_noresults_submit($form, &$form_state) {
  $entries = db_query('SELECT * FROM {cache_geocoder}');
  foreach ($entries as $entry) {
    $data = unserialize($entry->data);
    if (!$data['geometry']) {
      cache_clear_all($entry->cid, 'cache_geocoder', FALSE);
    }
  }

  drupal_set_message(t('No-results Geocoder cache cleared.'));
}
