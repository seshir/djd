<?php
/**
 * @file
 * Abstracts the Developer object in the Management API and allows clients to
 * manipulate it.
 *
 * @author djohnson
 */

namespace Apigee\ManagementAPI;

use \Apigee\Exceptions\ResponseException;
use \Apigee\Exceptions\ParameterException;
use \Apigee\Util\APIClient;

class Developer extends Base implements DeveloperInterface {

  /**
   * @var array
   */
  protected $apps;
  /**
   * @var string
   * This is actually the unique-key (within the org) for the Developer
   */
  protected $email;
  /**
   * @var string
   * Read-only alternate unique ID. Useful when querying developer analytics.
   */
  protected $developerId;
  /**
   * @var string
   */
  protected $firstName;
  /**
   * @var string
   */
  protected $lastName;
  /**
   * @var string
   */
  protected $userName;
  /**
   * @var string
   * Read-only
   */
  protected $orgName;
  /**
   * @var string
   * Should be either 'active' or 'inactive'.
   */
  protected $status;
  /**
   * @var array
   * This must be protected because Base wants to twiddle with it.
   */
  protected $attributes;
  /**
   * @var int
   * Read-only
   */
  protected $createdAt;
  /**
   * @var string
   * Read-only
   */
  protected $createdBy;
  /**
   * @var int
   * Read-only
   */
  protected $modifiedAt;
  /**
   * @var string
   * Read-only
   */
  protected $modifiedBy;

  protected $baseUrl;

  /* Accessors (getters/setters) */
  public function getApps() {
    return $this->apps;
  }
  public function getEmail() {
    return $this->email;
  }
  public function setEmail($email) {
    $this->email = $email;
  }
  public function getDeveloperId() {
    return $this->developerId;
  }
  public function getFirstName() {
    return $this->firstName;
  }
  public function setFirstName($fname) {
    $this->firstName = $fname;
  }
  public function getLastName() {
    return $this->lastName;
  }
  public function setLastName($lname) {
    $this->lastName = $lname;
  }
  public function getUserName() {
    return $this->userName;
  }
  public function setUserName($uname) {
    $this->userName = $uname;
  }
  public function getStatus() {
    return $this->status;
  }
  public function setStatus($status) {
    if ($status === 0 || $status === FALSE) {
      $status = 'inactive';
    }
    elseif ($status === 1 || $status === TRUE) {
      $status = 'active';
    }
    if ($status != 'active' && $status != 'inactive') {
      throw new ParameterException('Status may be either active or inactive; value "' . $status . '" is invalid.');
    }
    $this->status = $status;
  }
  public function getAttribute($attr) {
    if (array_key_exists($attr, $this->attributes)) {
      return $this->attributes[$attr];
    }
    return NULL;
  }
  public function setAttribute($attr, $value) {
    $this->attributes[$attr] = $value;
  }
  public function getAttributes() {
    return $this->attributes;
  }
  public function getModifiedAt() {
    return $this->modifiedAt;
  }

  /**
   * Initializes default values of all member variables.
   *
   * @param \Apigee\Util\APIClient $client
   */
  public function __construct(APIClient $client) {
    $this->init($client);
    $this->baseUrl = '/organizations/' . $this->urlEncode($client->getOrg()) . '/developers';
    $this->blankValues();
  }

  /**
   * Loads a developer from the Management API using $email as the unique key.
   *
   * @param $email
   */
  public function load($email) {
    $url = $this->baseUrl . '/' . $this->urlEncode($email);
    $this->client->get($url);
    $developer = $this->getResponse();
    $this->apps = $developer['apps'];
    $this->email = $developer['email'];
    $this->developerId = $developer['developerId'];
    $this->firstName = $developer['firstName'];
    $this->lastName = $developer['lastName'];
    $this->userName = $developer['userName'];
    $this->orgName = $developer['orgName'];
    $this->status = $developer['status'];
    $this->attributes = array();
    foreach ($developer['attributes'] as $attribute) {
      $this->attributes[$attribute['name']] = $attribute['value'];
    }
    $this->createdAt = $developer['createdAt'];
    $this->createdBy = $developer['createdBy'];
    $this->modifiedAt = $developer['lastModifiedAt'];
    $this->modifiedBy = $developer['lastModifiedBy'];
  }

  /**
   * Attempts to load developer from Management API. Returns TRUE if load was
   * successful.
   *
   * If $email is not supplied, the result will always be FALSE.
   *
   * As a bit of trivia, the $email parameter may either be the actual
   * developer email, or it can be a developer_id.
   *
   * @param null|string $email
   * @return bool
   */
  public function validate($email = NULL) {
    if (isset($email)) {
      $url = $this->baseUrl . '/' . $this->urlEncode($email);
      try {
        $this->client->get($url);
        return $this->client->wasSuccessful();
      }
      catch (ResponseException $e) { }
    }
    return FALSE;
  }

  /**
   * Saves user data to the Management API. This operates as both insert and
   * update.
   *
   * If user's email doesn't look valid (must contain @), a
   * ParameterException is thrown.
   *
   * @throws \Apigee\Exceptions\ParameterException
   */
  public function save($force_update = FALSE) {
    if (!$this->validateUser()) {
      throw new ParameterException('Invalid email address; cannot save user.');
    }

    $payload = array(
      'email' => $this->email,
      'userName' => $this->userName,
      'firstName' => $this->firstName,
      'lastName' => $this->lastName,
      'status' => $this->status,
    );
    if (count($this->attributes) > 0) {
      $payload['attributes'] = array();
      foreach ($this->attributes as $name => $value) {
        $payload['attributes'][] = array('name' => $name, 'value' => $value);
      }
    }
    $url = $this->baseUrl;
    if ($force_update || $this->createdAt) {
      if ($this->developerId) {
        $payload['developerId'] = $this->developerId;
      }
      $url .= '/' . $this->urlEncode($this->email);
    }
    if ($force_update) {
      $this->client->put($url, $payload);
    }
    else {
      $this->client->post($url, $payload);
    }
    $this->getResponse();
  }

  /**
   * Deletes a developer.
   *
   * If $email is not supplied, $this->email is used.
   *
   * @param null|string $email
   */
  public function delete($email = NULL) {
    if (!isset($email)) {
      $email = $this->email;
    }
    $this->client->delete($this->baseUrl . '/' . $this->urlEncode($email));
    $this->getResponse();
    if ($email == $this->email) {
      $this->blankValues();
    }
  }

  /**
   * Returns an array of all developer emails for this org.
   *
   * @return array
   */
  public function listDevelopers() {
    $this->client->get($this->baseUrl);
    $developers = $this->getResponse();
    return $developers;
  }

  /**
   * Ensures that current developer's email looks at least sort of valid.
   *
   * If first name and/or last name are not supplied, they are auto-
   * populated based on email. This is kind of shoddy but it's the best we can
   * do.
   *
   * @return bool
   */
  public function validateUser() {
    if (!empty($this->email) && (strpos($this->email, '@') > 0)) {
      $name = explode('@', $this->email, 2);
      if (empty($this->firstName)) {
        $this->firstName = $name[0];
      }
      if (empty($this->lastName)) {
        $this->lastName = $name[1];
      }
      return TRUE;
    }
    else {
      return FALSE;
    }
  }

  /**
   * Populates this object's properties based on a Drupal user object.
   *
   * Be aware that previous properties are not blanked first. If you are
   * creating a new user, you may want to call $this->blank_values() first.
   *
   * @deprecated
   *
   * @param $account
   */
  public function populateFromUserAccount($account) {
    $this->email = $account->mail;
    $this->firstName = $account->field_first_name[LANGUAGE_NONE][0]['value'];
    $this->lastName = $account->field_last_name[LANGUAGE_NONE][0]['value'];
    $this->userName = $account->name;
    $this->status = ($account->status ? 'active' : 'inactive');

    $vars = get_object_vars($account);
    foreach ($vars as $key => $value) {
      if (substr($key, 0, 10) == 'attribute_') {
        $this->attributes[substr($key, 10)] = $value;
      }
    }
  }

  /**
   * Restores this object's properties to their pristine state.
   */
  public function blankValues() {
    $this->apps = array();
    $this->email = NULL;
    $this->developerId = NULL;
    $this->firstName = NULL;
    $this->lastName = NULL;
    $this->userName = NULL;
    $this->orgName = NULL;
    $this->status = NULL;
    $this->attributes = array();
    $this->createdAt = NULL;
    $this->createdBy = NULL;
    $this->modifiedAt = NULL;
    $this->modifiedBy = NULL;
  }
}