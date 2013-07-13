using System;
using IGSMLib;

namespace IGPE
{
    public static class AppInitializer
    {
        public static void AppInitialize()
        {            
            //
            // IMPORTANT! 
            //
            // Since MembershipUserSecurityAuthority under App_Code gets compiled
            // into a dynamically generated assembly, its name cannot be known 
            // at design-time and therefore specified in web.config. The 
            // following type resolution hook takes care of this problem. In
            // web.config, you can simply specify
            // "MembershipUserSecurityAuthority" for the provider attribute of
            // the <userSecurityAuthority> configuration element:
            //
            //  <userSecurityAuthority 
            //      realm="MADAM" 
            //      provider="MembershipUserSecurityAuthority" 
            //      exposeClearTextPasswords="true" />
            //
            // The reason why type resolution fails is that MADAM was compiled
            // against runtime version 1.1 where it uses Type.GetType on the
            // provider attribute. In the absence of an assembly name, 
            // Type.GetType will search the calling assembly (MADAM) and 
            // mscorlib only, which obviously fails for dynamic code and the 
            // new compilation model in ASP.NET 2.0. In ASP.NET 2.0, one is asked 
            // to use BuildManager.GetType instead for resolving types. The 
            // following hook therefore accomodates this change without needing
            // to re-compile MADAM for runtime version 2.0.
            //

            AppDomain.CurrentDomain.TypeResolve += delegate(object sender, ResolveEventArgs args)
            {
                //
                // NOTE: We could also use BuildManager.GetType here.
                // 

                if (typeof(MembershipUserSecurityAuthority).FullName.Equals(args.Name))
                    return typeof(MembershipUserSecurityAuthority).Assembly;

                return null;
            };
        }
    }
}