package br.newm.inspector

import android.content.Context

object Inspector {

    private var context: Context? = null

    init {
        System.loadLibrary("inspector")
    }

    @JvmStatic
    fun initializeWithDefaults(context: Context) {
        this.context = context

        initialize()
    }

    @JvmStatic
    fun databaseList(): Array<String> {
        return context!!.databaseList().map { context!!.getDatabasePath(it).absolutePath }.toTypedArray()
    }

    private external fun initialize()
}
