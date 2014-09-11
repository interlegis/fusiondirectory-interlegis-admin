<?php
/* Common functions for Interlegis Class */

/* Gera um novo codigo Interlegis */

function generateInterlegisCode($cityName, $ldap)
{
  $cityName = preg_replace( '/[`^~\'"]/', null, iconv( 'UTF-8', 'ASCII//TRANSLIT', $cityName ) );
  $cityTmp = strtoupper($cityName);
  $cityTmp = str_replace(' DA ',' ',$cityTmp);
  $cityTmp = str_replace(' DE ',' ',$cityTmp);
  $cityTmp = str_replace(' DO ',' ',$cityTmp);

  # estrategia 1 - Pegar a primeira letra de cada nome da cidade
  $cityParts = explode(' ',$cityTmp);
  $codigo = '';
  for ($i = 0; $i <= sizeof($cityParts) and $i < 3; $i++) {
    $codigo = $codigo.$cityParts[$i][0];
  }

  # Se o codigo ficou com menos que tres letras, pegar as 2 primeiras
  if (strlen($codigo) < 3) {
    $codigo = '';
    for ($i = 0; $i <= sizeof($cityParts); $i++) {
       $codigo = $codigo.$cityParts[$i][0].$cityParts[$i][1];
    }
    $codigo = substr($codigo,0,3);
  }

  # Se ainda ficou com menos de tres letras, entao o nome da cidade so
  # tem uma palavra. Pegue as tres primeiras letras da palavra  
  if (strlen($codigo) < 3) {
    $codigo = substr($cityTmp,0,3);
  }

  # Se o codigo ja existir, substituir a ultima letra do codigo pela
  # ultima letra do nome da cidade, e ir recuando, letra a letra,
  # até achar um novo codigo.
  $cityTmp = str_replace(' ','',$cityTmp);
  $ultima = strlen($cityTmp);

  while ( count_interlegis_codes($ldap,$codigo) > 0 and $ultima > 0) {
    $codigo = substr($codigo,0,2).substr($cityTmp,$ultima-1,1);
    $ultima--;
  }

  # Se ainda nao gerou um nome unico, vamos colocar dígitos no
  # ultimo caractere, de A a Z
  for ($i=ord('A'); count_interlegis_codes($ldap,$codigo) > 0 and $i <= ord('Z'); $i++) {
    $codigo = substr($codigo,0,2).chr($i);
  }

  # Se nao encontrou, comece a gerar strings com 3 letras aleatorias
  # tiradas do nome da cidade, até gerar uma que nao existe. Tentar
  # 100 vezes apenas
  for ($i=0; count_interlegis_codes($ldap,$codigo) > 0 and $i<100; $i++) {
    $codigo = substr(str_shuffle($cityTmp),0,3);
  }

  # Caramba! So resta entao gerar o codigo com 3 letras aleatorias
  # quaisquer do alfabeto!
  $alpha='ABCDEFGHIJKLMNOPQRSTUVWXYZ';
  for ($i=0; count_interlegis_codes($ldap,$codigo) > 0; $i++) {
    $codigo = substr(str_shuffle($alpha),0,3);
  }

  return $codigo;
}

/* Conta quantos objetos possuem um determinado codigo Interlegis */
function count_interlegis_codes($ldap, $code) {
  $ldap->search('(interlegisCode='.$code.')', array('cn'));
  return $ldap->count();
}

function decide_instance($ldap, $objectType, $perInstance = 25) {
   $instances = get_zope_instances($ldap, $objectType);
   $theinstance = null;
   foreach ( $instances as $instance ) {
     $num = sizeof(get_instance_objects($ldap, $objectType, $instance));
     if ( $num < $perInstance ) {
       $theinstance = $instance;
       break; 
     }
   }
   return $theinstance; 
}

function get_instance_objects($ldap, $objectType, $instance) { 
   $ldap->search('(&(objectClass='.$objectType.')(instanceGroup='.$instance.'))', array('interlegisCode'));
   $objects = array();
   while ($attrs = $ldap->fetch()){
      for ($i=0;$i < sizeof($attrs['interlegisCode'])-1; $i++){
        $objects[sizeof($objects)] = $attrs['interlegisCode'][$i];
      }
   } 
   return $objects;
}

function get_zope_instances($ldap, $objectType) {
   //lets get the DB instances
   $ldap->search('(&(objectClass='.$objectType.'PrimaryDBServer)(instanceGroup=*))', array('instanceGroup'));
   $dbinstances = array();
   while ($attrs = $ldap->fetch()){
      for ($i=0;$i < sizeof($attrs['instanceGroup'])-1; $i++){
        $dbinstances[sizeof($dbinstances)] = $attrs['instanceGroup'][$i];
      }
   } 

   //now lets get the Application Server Instances
   $ldap->search('(&(objectClass='.$objectType.'AppServer)(instanceGroup=*))', array('instanceGroup'));
   $appinstances = array();
   while ($attrs = $ldap->fetch()){
      for ($i=0;$i < sizeof($attrs['instanceGroup'])-1; $i++){
        $appinstances[sizeof($appinstances)] = $attrs['instanceGroup'][$i];
      }
   }
   
   $instances = array_intersect($dbinstances, $appinstances);
   asort($instances);
   return $instances;
}

?>
