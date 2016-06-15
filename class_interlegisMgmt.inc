<?php
/*
  Interlegis 

*/

class interlegisMgmt extends simpleManagement
{
  // Tab definitions
  protected $tabClass     = "simpleTabs";
  protected $aclCategory  = "Interlegis";
  protected $aclPlugin    = "interlegis";
  protected $objectName   = "interlegis";
  protected $objectTypes  = array('portalModelo', 'sapl');
  protected $autoFilterAttributes = array('dn', 
                                          'cn', 
                                          'city', 
                                          'interlegisCode', 
                                          'instanceGroup',
                                          'activationDate');

  protected $departmentBrowser      = TRUE;
  protected $departmentRootVisible  = TRUE;
  protected $baseMode               = TRUE;
  protected $multiSelect            = TRUE;
  protected $autoFilter             = TRUE;
  protected $filterXMLPath          = NULL;
  protected $listXMLPath            = NULL;

  /* Return plugin informations for acl handling  */
  public static function plInfo()
  {
    return array(
      "plShortName"    => _("Interlegis"),
      "plDescription"  => _("Interlegis Products"),
      "plIcon"         => "plugins/interlegis/images/icon.png",
      "plSelfModify"   => FALSE,
      "plSection"      => 'systems',
      "plPriority"     => 4,
      "plCategory"     => array("Interlegis" => array("description"  => _("Interlegis Products"),
                                                     "objectClass"  => array( "portalModelo", "sapl")
                                               )
                         ),
      "plManages"      => array('portalModelo', 'sapl'),
      "plProvidedAcls" => array()
    );
  }

  function __construct()
  {
    $this->filterXMLPath  = get_template_path('interlegis-filter.xml',  TRUE, dirname(__FILE__));
    $this->listXMLPath    = get_template_path('interlegis-list.xml',    TRUE, dirname(__FILE__));
    $this->storagePoints  = array("ou=interlegis");

    parent::__construct();

  }

  function configureHeadpage ()
  {
    $this->headpage->registerElementFilter("filterDate", "interlegisMgmt::filterDate");
    parent::configureHeadpage();
    $this->registerAction("edit_interlegis", "editEntry");
    $this->registerAction("new_portalModelo", "newEntry");
    $this->registerAction("new_sapl", "newEntry");
  }

  // Overridden new handler - We've different types of departments to create!
  function newEntry($action = "", $target = array(), $all = array(), $altTabClass = "",
                    $altTabType = "", $altAclCategory = "")
  {
    $type = preg_replace("/^new_/", "", $action);

    return management::newEntry($action, $target, $all, $this->tabClass,
                                strtoupper($type).'TABS', $this->aclCategory);
  }

  function editEntry( $action = "", $target = array(), $all = array(), $altTabClass = "",
                      $altTabType = "", $altAclCategory = "")
  {
    $type = $this->getHeadpage()->getType($target[0]);

    $str = management::editEntry( $action, $target, $all, $this->tabClass,
                                  strtoupper($type).'TABS', $this->aclCategory);
    if (preg_match("/^edit_/", $action)) {
      $tab = preg_replace("/^edit_/", "", $action);
      if (isset($this->tabObject->by_object[$tab])) {
        $this->tabObject->current = $tab;
      } else {
        trigger_error("Unknown tab: ".$tab);
      }
    }
    if (!empty($str)) {
      return $str;
    }
  }

  function removeEntryConfirmed($action = "", $target = array(), $all = array(),
                                $altTabClass = "", $altTabType = "", $altAclCategory = "")
  {
    $headpage = $this->getHeadpage();

    $targets = array();
    foreach ($this->dns as $dn) {
      $type = $headpage->getType($dn);
      if (empty($type)) {
        continue;
      }
      $targets[$type][] = $dn;
    }

    foreach ($targets as $type => $dns) {
      $this->dns = $dns;
      management::removeEntryConfirmed($action, $target, $all, $this->tabClass,
                                  strtoupper($type).'TABS', $this->aclCategory);
    }
  }

  static function filterDate()
  {
    if (func_num_args() == 0) {
      return '&nbsp;';
    }
    $date = func_get_arg(0);
    if (!is_array($date)) {
      return '&nbsp;';
    }
    $dateObject = DateTime::createFromFormat('U', $date[0], new DateTimeZone('UTC'));
    if (is_object($dateObject)) {
      return $dateObject->format('d/m/Y');
    }
    return '&nbsp;';
  }
}
?>
