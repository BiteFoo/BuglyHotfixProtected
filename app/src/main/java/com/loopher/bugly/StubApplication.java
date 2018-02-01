package com.loopher.bugly;

import com.tencent.tinker.loader.app.TinkerApplication;
import com.tencent.tinker.loader.shareutil.ShareConstants;

/**
 * Created by John.Lu on 2018/1/30.
 */

public class StubApplication extends TinkerApplication {
    public StubApplication() {
        super(ShareConstants.TINKER_ENABLE_ALL,
                "com.loopher.bugly.MyApplicationLike",
                "com.tencent.tinker.loader.TinkerLoader",
                false);
    }
}
