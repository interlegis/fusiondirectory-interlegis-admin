<?php
/*
  Interlegis 

*/

class interlegisMgmt extends simpleManagement
{
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

  function configureHeadpage ()
  {
    $this->headpage->registerElementFilter('filterDate', 'interlegisMgmt::filterDate');
    parent::configureHeadpage();
  }



  function parseXML ($file)
  {
    $data = parent::parseXML($file);
    $data['list']['table']['layout'] = '|20px;c|||||110px;c|140px;r|';
    $data['list']['table']['column'][2] = array(
      'label'         => 'City',
      'sortAttribute' => 'city',
      'sortType'      => 'string',
      'value'         => '%{filter:link(row,dn,"%s",city)}',
      'export'        => 'true',
    );
    $data['list']['table']['column'][3] = array(
      'label'         => 'Interlegis code',
      'sortAttribute' => 'interlegisCode',
      'sortType'      => 'string',
      'value'         => '%{filter:link(row,dn,"%s",interlegisCode)}',
      'export'        => 'true',
    );
    $data['list']['table']['column'][4] = array(
      'label'         => 'Inst. Group',
      'sortAttribute' => 'instanceGroup',
      'sortType'      => 'integer',
      'value'         => '%{filter:link(row,dn,"%d",instanceGroup)}',
      'export'        => 'true',
    );
    $data['list']['table']['column'][5] = array(
      'label'         => 'Activation date',
      'sortAttribute' => 'activationDate',
      'sortType'      => 'integer',
      'value'         => '%{filter:filterDate(activationDate)}',
      'export'        => 'true',
    );
    $data['list']['table']['column'][6] = array(
      'label'         => 'Actions',
      'value'         => '%{filter:actions(dn,row,objectClass)}',
    );
    return $data;
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
