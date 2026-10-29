package com.gelb2.heavymach
import android.content.BroadcastReceiver
import android.content.Context
import android.content.Intent
/** Shell-only validation messages, delivered without restarting/resuming Activity. */
class TestReceiver : BroadcastReceiver() {
 override fun onReceive(context:Context,intent:Intent){
  if(intent.action!="com.gelb2.heavymach.TEST")return
  val command=intent.getStringExtra("command")?:return
  val pending=goAsync()
  GameEngine.command(command,intent.getIntExtra("a",0),intent.getIntExtra("b",0),intent.getIntExtra("c",0),intent.getIntExtra("d",0))
  GameEngine.afterCommands{pending.finish()}
 }
}
