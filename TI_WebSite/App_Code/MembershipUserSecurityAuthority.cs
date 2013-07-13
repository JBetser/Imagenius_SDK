#region Imports

using System;
using System.Collections;
using System.Web;
using System.Web.Security;
using IGMadam;

#endregion

public sealed class MembershipUserSecurityAuthority : UserSecurityAuthorityBase
{
    private string _membershipProviderName;

    public MembershipUserSecurityAuthority() {}

    public MembershipUserSecurityAuthority(IDictionary settings)
    {
        if (settings == null)
            return;

        //
        // Read the membership provider name and ensure that it is a 
        // valid provider.
        //

        _membershipProviderName = (string)settings["membershipProvider"];

        if (!string.IsNullOrEmpty(_membershipProviderName) && Membership.Providers[_membershipProviderName] == null)
            throw new ArgumentException(string.Format("Membership provider '{0}' does not exist.", _membershipProviderName));
    }

    public MembershipProvider MembershipProvider
    {
        get
        {
            if (string.IsNullOrEmpty(_membershipProviderName))
                return Membership.Provider;
            else
                return Membership.Providers[_membershipProviderName];
        }
    }

    protected override bool ValidateUser(string userName, string password)
    {
        return MembershipProvider.ValidateUser(userName, password);
    }

    protected override string GetPassword(string userName)
    {
        return MembershipProvider.GetPassword(userName, null);
    }
    
    public override string GetUserPrivilege(string userName)
    {
        return null;
    }

    public override string GetUserShortName(string userName)
    {
        return null;
    }
}
