<?php
/*
  Interlegis 

*/

class interlegisMgmt extends management
{
  // Tab definitions
  protected $tabClass     = "simpleTabs";
  protected $aclCategory  = "Interlegis";
  protected $aclPlugin    = "interlegis";
  protected $objectName   = "interlegis";

  protected $departmentBrowser      = TRUE;
  protected $departmentRootVisible  = TRUE;
  protected $baseMode               = TRUE;
  protected $multiSelect            = TRUE;


  /* Return plugin informations for acl handling  */
  public static function plInfo()
  {
    return array(
      "plShortName"    => _("Interlegis"),
      "plDescription"  => _("Interlegis Products"),
      "plIcon"         => "plugins/interlegis/images/icon.png",
      "plSelfModify"   => FALSE,
      "plSection"      => "admin",
      "plPriority"     => 4,
      "plCategory"     => array("Interlegis" => array("description"  => _("Interlegis Products"),
                                                     "objectClass"  => array( "portalModelo", "sapl")
                                               )
                         ),
      "plManages"      => array('portalModelo', 'sapl'),
      "plProvidedAcls" => array()
    );
  }

  function __construct($config, $ui)
  {
    $this->config = $config;
    $this->ui     = $ui;

    $this->storagePoints = array("ou=interlegis");

    // Build filter
    if (session::global_is_set(get_class($this)."_filter")) {
      $filter = session::global_get(get_class($this)."_filter");
    } else {
      $filter = new filter(get_template_path("interlegis-filter.xml", TRUE));
      $filter->setObjectStorage($this->storagePoints);
    }
    $this->setFilter($filter);

    // Build headpage
    $headpage = new listing(get_template_path("interlegis-list.xml", TRUE));
    $headpage->registerElementFilter("filterDate", "interlegisMgmt::filterDate");
    $headpage->setFilter($filter);

    parent::__construct($config, $ui, "interlegis", $headpage);

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

  /*! \brief  This method is used to queue and process copy&paste actions.
   *          Allows to copy, cut and paste mutliple entries at once.
   *  @param  String  'action'  The name of the action which was the used as trigger.
   *  @param  Array   'target'  A list of object dns, which should be affected by this method.
   *  @param  Array   'all'     A combination of both 'action' and 'target'.
   */
  function copyPasteHandler($action = "", $target = array(), $all = array(),
      $altTabClass = "", $altTabType = "", $altAclCategory = "", $altAclPlugin = "")
  {
    // Add entries to queue
    if ($action == "copy" || $action == "cut") {
      $headpage = $this->getHeadpage();

      $targets = array();
      foreach ($target as $dn) {
        $type = $headpage->getType($dn);
        if (empty($type)) {
          continue;
        }
        $targets[$type][] = $dn;
      }

      foreach ($targets as $type => $dns) {
        return management::copyPasteHandler($action, $dns, $all, $this->tabClass,
                                    strtoupper($type).'TABS', $this->aclCategory);
      }
    } else {
      return management::copyPasteHandler($action, $target, $all);
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
